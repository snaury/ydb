#pragma once
#include "external_data.h"
#include "tier_config.h"

#include <library/cpp/actors/core/actor_bootstrapped.h>
#include <library/cpp/actors/core/actor.h>
#include <ydb/services/metadata/service.h>
#include <ydb/library/accessor/accessor.h>

namespace NKikimr::NColumnShard {
namespace NTiers {

class TManager {
private:
    ui64 TabletId = 0;
    YDB_READONLY_DEF(TTierConfig, Config);
    YDB_READONLY_DEF(NActors::TActorId, StorageActorId);
public:
    TManager(const ui64 tabletId, const TTierConfig& config);
    static NOlap::TCompression ConvertCompression(const NKikimrSchemeOp::TCompressionOptions& compression);
    NOlap::TStorageTier BuildTierStorage() const;

    TManager& Restart(const TTierConfig& config, const bool activity);
    bool NeedExport() const {
        return Config.NeedExport();
    }
    bool Stop();
    bool Start();
};
}

class TTiersManager {
private:
    class TActor;
    using TManagers = TMap<TString, NTiers::TManager>;
    ui64 TabletId = 0;
    TString OwnerPath;
    TActor* Actor = nullptr;
    YDB_READONLY_DEF(TManagers, Managers);
    YDB_READONLY_FLAG(Active, false);

    NMetadataProvider::ISnapshot::TPtr Snapshot;
    mutable NMetadataProvider::ISnapshotParser::TPtr ExternalDataManipulation;

public:
    TTiersManager(const ui64 tabletId, const TString& ownerPath)
        : TabletId(tabletId)
        , OwnerPath(ownerPath)
    {
    }
    ~TTiersManager();
    THashMap<ui64, NOlap::TTiersInfo> GetTiering() const;
    void TakeConfigs(NMetadataProvider::ISnapshot::TPtr snapshot);
    TTiersManager& Start(std::shared_ptr<TTiersManager> ownerPtr);
    TTiersManager& Stop();
    TActorId GetStorageActorId(const TString& tierName);
    const NTiers::TManager& GetManagerVerified(const TString& tierId) const;
    NMetadataProvider::ISnapshotParser::TPtr GetExternalDataManipulation() const;

    TManagers::const_iterator begin() const {
        return Managers.begin();
    }

    TManagers::const_iterator end() const {
        return Managers.end();
    }
};

}
