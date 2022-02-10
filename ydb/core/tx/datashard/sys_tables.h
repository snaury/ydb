#pragma once 
#include <ydb/core/scheme/scheme_types_auto.h>
#include <ydb/core/scheme/scheme_tabledefs.h>
 
#include <util/system/unaligned_mem.h>

namespace NKikimr { 
 
struct TSysTables { 
    struct TTableColumnInfo {
        TString Name;
        ui32 Id = 0;
        ui32 PType = 0;
        i32 KeyOrder = -1;
 
        TTableColumnInfo() = default;

        TTableColumnInfo(TString name, ui32 colId, ui32 type, i32 keyOrder = -1)
            : Name(name)
            , Id(colId)
            , PType(type)
            , KeyOrder(keyOrder)
        {}
    };

    // fake TabletIds 
    enum ESysIds : ui64 { 
        SysSchemeShard = 1,     // fake SchemeShard 
        SysTableLocks2 = 997,   // /sys/locks2 
        SysTableLocks = 998,    // /sys/locks 
        SysTableMAX = 999 
    }; 
 
    static bool IsSystemTable(const TTableId& table) { return table.PathId.OwnerId == SysSchemeShard; }
    static bool IsLocksTable(const TTableId& table) {
        return IsSystemTable(table) && (table.PathId.LocalPathId == SysTableLocks || table.PathId.LocalPathId == SysTableLocks2);
    } 
 
    struct TLocksTable { 
        enum class EColumns { 
            LockId = 0, 
            DataShard, 
            Generation, 
            Counter, 
            SchemeShard, // real ss id 
            PathId, 
        }; 
 
        struct TLock { 
            enum ESetErrors : ui64 { 
                ErrorMin = Max<ui64>() - 255, 
                ErrorAlreadyBroken = Max<ui64>() - 3,
                ErrorNotSet = Max<ui64>() - 2, 
                ErrorTooMuch = Max<ui64>() - 1, 
                ErrorBroken = Max<ui64>() 
            }; 
 
            ui64 LockId = 0; 
            ui64 DataShard = 0; 
            ui32 Generation = 0; 
            // Padding is used to avoid use-of-uninitialized on writes to DB.
            ui32 Padding_ = 0;
            ui64 Counter = 0; 
            ui64 SchemeShard = 0; 
            ui64 PathId = 0; 
 
            bool IsEmpty() const { return (LockId == 0); } 
            bool IsError() const { return IsError(Counter); } 
 
            static bool IsError(ui64 counter) { return (counter >= ErrorMin); } 
            static bool IsNotSet(ui64 counter) { return (counter == ErrorNotSet); } 
            static bool IsTooMuch(ui64 counter) { return (counter == ErrorTooMuch); } 
            static bool IsBroken(ui64 counter) { return (counter == ErrorBroken); } 
 
            TVector<TCell> MakeRow(bool v2) const { 
                TVector<TCell> row;
                MakeRow(row, v2); 
                return row; 
            } 
 
            void MakeRow(TVector<TCell>& row, bool v2) const { 
                row.clear(); 
                if (!IsEmpty()) { 
                    row.reserve(6); 
                    row.emplace_back(TCell((const char*)&LockId, sizeof(ui64))); 
                    row.emplace_back(TCell((const char*)&DataShard, sizeof(ui64))); 
                    row.emplace_back(TCell((const char*)&Generation, sizeof(ui32))); 
                    row.emplace_back(TCell((const char*)&Counter, sizeof(ui64))); 
                    if (v2) { 
                        row.emplace_back(TCell((const char*)&SchemeShard, sizeof(ui64))); 
                        row.emplace_back(TCell((const char*)&PathId, sizeof(ui64))); 
                    } 
                } 
            } 
        }; 
 
        static const char * GetColName(EColumns colId) { 
            switch (colId) { 
            case EColumns::LockId: 
                return "LockId"; 
            case EColumns::DataShard: 
                return "DataShard"; 
            case EColumns::Generation: 
                return "Generation"; 
            case EColumns::Counter: 
                return "Counter"; 
            case EColumns::SchemeShard: 
                return "SchemeShard"; 
            case EColumns::PathId: 
                return "PathId"; 
            }; 
            Y_ASSERT("Unknown column"); 
            return ""; 
        } 
 
        static void GetInfo(THashMap<ui32, TTableColumnInfo>& columns, TVector<ui32>& keyTypes, bool v2) { 
            /// @warning Generation is uint32. Don't ask me why 
            auto type = NScheme::TUint64::TypeId; 
            columns[0] = TTableColumnInfo(GetColName(EColumns::LockId), (ui32)EColumns::LockId, type, 0); 
            columns[1] = TTableColumnInfo(GetColName(EColumns::DataShard), (ui32)EColumns::DataShard, type, 1); 
            columns[2] = TTableColumnInfo(GetColName(EColumns::Generation), (ui32)EColumns::Generation, NScheme::TUint32::TypeId); 
            columns[3] = TTableColumnInfo(GetColName(EColumns::Counter), (ui32)EColumns::Counter, type); 
 
            keyTypes.push_back(type); 
            keyTypes.push_back(type); 
 
            if (v2) { 
                columns[4] = TTableColumnInfo(GetColName(EColumns::SchemeShard), (ui32)EColumns::SchemeShard, type, 2); 
                columns[5] = TTableColumnInfo(GetColName(EColumns::PathId), (ui32)EColumns::PathId, type, 3); 
                keyTypes.push_back(type); 
                keyTypes.push_back(type); 
            } 
        } 
 
        static bool ExtractKey(const TArrayRef<const TCell>& key, EColumns columnId, ui64& value) { 
            Y_VERIFY(columnId == EColumns::LockId || 
                     columnId == EColumns::DataShard || 
                     columnId == EColumns::SchemeShard || 
                     columnId == EColumns::PathId); 
 
            ui32 keyPos = (ui32)columnId; 
            if (columnId == EColumns::SchemeShard || columnId == EColumns::PathId) 
                keyPos -= 2; 
 
            const TCell& cell = key[keyPos]; 
            if (cell.IsNull()) 
                return false; 
 
            Y_VERIFY(cell.Size() == sizeof(ui64)); 
            value = ReadUnaligned<ui64>(reinterpret_cast<const ui64*>(cell.Data()));
            return true; 
        } 
    }; 
}; 
 
 
inline IOutputStream& operator << (IOutputStream& out, const TSysTables::TLocksTable::TLock& lock) { 
    out << lock.LockId << ':' << lock.DataShard << ':' << lock.Generation << ':' << lock.Counter << ':' 
        << lock.SchemeShard << ':' << lock.PathId; 
    return out; 
} 
 
} 
