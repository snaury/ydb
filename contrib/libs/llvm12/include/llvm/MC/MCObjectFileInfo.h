#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

//===-- llvm/MC/MCObjectFileInfo.h - Object File Info -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes common object file formats.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_MCOBJECTFILEINFO_H
#define LLVM_MC_MCOBJECTFILEINFO_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/VersionTuple.h"

namespace llvm {
class MCContext;
class MCSection;

class MCObjectFileInfo {
protected:
  /// True if .comm supports alignment.  This is a hack for as long as we
  /// support 10.4 Tiger, whose assembler doesn't support alignment on comm.
  bool CommDirectiveSupportsAlignment = false;

  /// True if target object file supports a weak_definition of constant 0 for an
  /// omitted EH frame.
  bool SupportsWeakOmittedEHFrame = false;

  /// True if the target object file supports emitting a compact unwind section
  /// without an associated EH frame section.
  bool SupportsCompactUnwindWithoutEHFrame = false;

  /// OmitDwarfIfHaveCompactUnwind - True if the target object file
  /// supports having some functions with compact unwind and other with
  /// dwarf unwind.
  bool OmitDwarfIfHaveCompactUnwind = false;

  /// FDE CFI encoding. Controls the encoding of the begin label in the
  /// .eh_frame section. Unlike the LSDA encoding, personality encoding, and
  /// type encodings, this is something that the assembler just "knows" about
  /// its target
  unsigned FDECFIEncoding = 0;

  /// Compact unwind encoding indicating that we should emit only an EH frame.
  unsigned CompactUnwindDwarfEHFrameOnly = 0;

  /// Section directive for standard text.
  MCSection *TextSection = nullptr;

  /// Section directive for standard data.
  MCSection *DataSection = nullptr;

  /// Section that is default initialized to zero.
  MCSection *BSSSection = nullptr;

  /// Section that is readonly and can contain arbitrary initialized data.
  /// Targets are not required to have a readonly section. If they don't,
  /// various bits of code will fall back to using the data section for
  /// constants.
  MCSection *ReadOnlySection = nullptr;

  /// If exception handling is supported by the target, this is the section the
  /// Language Specific Data Area information is emitted to.
  MCSection *LSDASection = nullptr;

  /// If exception handling is supported by the target and the target can
  /// support a compact representation of the CIE and FDE, this is the section
  /// to emit them into.
  MCSection *CompactUnwindSection = nullptr;

  // Dwarf sections for debug info.  If a target supports debug info, these must
  // be set.
  MCSection *DwarfAbbrevSection = nullptr;
  MCSection *DwarfInfoSection = nullptr;
  MCSection *DwarfLineSection = nullptr;
  MCSection *DwarfLineStrSection = nullptr;
  MCSection *DwarfFrameSection = nullptr;
  MCSection *DwarfPubTypesSection = nullptr;
  const MCSection *DwarfDebugInlineSection = nullptr;
  MCSection *DwarfStrSection = nullptr;
  MCSection *DwarfLocSection = nullptr;
  MCSection *DwarfARangesSection = nullptr;
  MCSection *DwarfRangesSection = nullptr;
  MCSection *DwarfMacinfoSection = nullptr;
  MCSection *DwarfMacroSection = nullptr;
  // The pubnames section is no longer generated by default.  The generation
  // can be enabled by a compiler flag.
  MCSection *DwarfPubNamesSection = nullptr;

  /// Accelerator table sections. DwarfDebugNamesSection is the DWARF v5
  /// accelerator table, while DwarfAccelNamesSection, DwarfAccelObjCSection,
  /// DwarfAccelNamespaceSection, DwarfAccelTypesSection are pre-DWARF v5
  /// extensions.
  MCSection *DwarfDebugNamesSection = nullptr;
  MCSection *DwarfAccelNamesSection = nullptr;
  MCSection *DwarfAccelObjCSection = nullptr;
  MCSection *DwarfAccelNamespaceSection = nullptr;
  MCSection *DwarfAccelTypesSection = nullptr;

  // These are used for the Fission separate debug information files.
  MCSection *DwarfInfoDWOSection = nullptr;
  MCSection *DwarfTypesDWOSection = nullptr;
  MCSection *DwarfAbbrevDWOSection = nullptr;
  MCSection *DwarfStrDWOSection = nullptr;
  MCSection *DwarfLineDWOSection = nullptr;
  MCSection *DwarfLocDWOSection = nullptr;
  MCSection *DwarfStrOffDWOSection = nullptr;
  MCSection *DwarfMacinfoDWOSection = nullptr;
  MCSection *DwarfMacroDWOSection = nullptr;

  /// The DWARF v5 string offset and address table sections.
  MCSection *DwarfStrOffSection = nullptr;
  MCSection *DwarfAddrSection = nullptr;
  /// The DWARF v5 range list section.
  MCSection *DwarfRnglistsSection = nullptr;
  /// The DWARF v5 locations list section.
  MCSection *DwarfLoclistsSection = nullptr;

  /// The DWARF v5 range and location list sections for fission.
  MCSection *DwarfRnglistsDWOSection = nullptr;
  MCSection *DwarfLoclistsDWOSection = nullptr;

  // These are for Fission DWP files.
  MCSection *DwarfCUIndexSection = nullptr;
  MCSection *DwarfTUIndexSection = nullptr;

  /// Section for newer gnu pubnames.
  MCSection *DwarfGnuPubNamesSection = nullptr;
  /// Section for newer gnu pubtypes.
  MCSection *DwarfGnuPubTypesSection = nullptr;

  // Section for Swift AST
  MCSection *DwarfSwiftASTSection = nullptr;

  MCSection *COFFDebugSymbolsSection = nullptr;
  MCSection *COFFDebugTypesSection = nullptr;
  MCSection *COFFGlobalTypeHashesSection = nullptr;

  /// Extra TLS Variable Data section.
  ///
  /// If the target needs to put additional information for a TLS variable,
  /// it'll go here.
  MCSection *TLSExtraDataSection = nullptr;

  /// Section directive for Thread Local data. ELF, MachO, COFF, and Wasm.
  MCSection *TLSDataSection = nullptr; // Defaults to ".tdata".

  /// Section directive for Thread Local uninitialized data.
  ///
  /// Null if this target doesn't support a BSS section. ELF and MachO only.
  MCSection *TLSBSSSection = nullptr; // Defaults to ".tbss".

  /// StackMap section.
  MCSection *StackMapSection = nullptr;

  /// FaultMap section.
  MCSection *FaultMapSection = nullptr;

  /// Remarks section.
  MCSection *RemarksSection = nullptr;

  /// EH frame section.
  ///
  /// It is initialized on demand so it can be overwritten (with uniquing).
  MCSection *EHFrameSection = nullptr;

  /// Section containing metadata on function stack sizes.
  MCSection *StackSizesSection = nullptr;

  /// Section for pseudo probe information used by AutoFDO 
  MCSection *PseudoProbeSection = nullptr; 
  MCSection *PseudoProbeDescSection = nullptr; 
 
  // ELF specific sections.
  MCSection *DataRelROSection = nullptr;
  MCSection *MergeableConst4Section = nullptr;
  MCSection *MergeableConst8Section = nullptr;
  MCSection *MergeableConst16Section = nullptr;
  MCSection *MergeableConst32Section = nullptr;

  // MachO specific sections.

  /// Section for thread local structure information.
  ///
  /// Contains the source code name of the variable, visibility and a pointer to
  /// the initial value (.tdata or .tbss).
  MCSection *TLSTLVSection = nullptr; // Defaults to ".tlv".

  /// Section for thread local data initialization functions.
   // Defaults to ".thread_init_func".
  const MCSection *TLSThreadInitSection = nullptr;

  MCSection *CStringSection = nullptr;
  MCSection *UStringSection = nullptr;
  MCSection *TextCoalSection = nullptr;
  MCSection *ConstTextCoalSection = nullptr;
  MCSection *ConstDataSection = nullptr;
  MCSection *DataCoalSection = nullptr;
  MCSection *ConstDataCoalSection = nullptr;
  MCSection *DataCommonSection = nullptr;
  MCSection *DataBSSSection = nullptr;
  MCSection *FourByteConstantSection = nullptr;
  MCSection *EightByteConstantSection = nullptr;
  MCSection *SixteenByteConstantSection = nullptr;
  MCSection *LazySymbolPointerSection = nullptr;
  MCSection *NonLazySymbolPointerSection = nullptr;
  MCSection *ThreadLocalPointerSection = nullptr;

  /// COFF specific sections.
  MCSection *DrectveSection = nullptr;
  MCSection *PDataSection = nullptr;
  MCSection *XDataSection = nullptr;
  MCSection *SXDataSection = nullptr;
  MCSection *GFIDsSection = nullptr;
  MCSection *GIATsSection = nullptr; 
  MCSection *GLJMPSection = nullptr;

  // XCOFF specific sections
  MCSection *TOCBaseSection = nullptr;

public:
  void InitMCObjectFileInfo(const Triple &TT, bool PIC, MCContext &ctx,
                            bool LargeCodeModel = false);
  MCContext &getContext() const { return *Ctx; }

  bool getSupportsWeakOmittedEHFrame() const {
    return SupportsWeakOmittedEHFrame;
  }
  bool getSupportsCompactUnwindWithoutEHFrame() const {
    return SupportsCompactUnwindWithoutEHFrame;
  }
  bool getOmitDwarfIfHaveCompactUnwind() const {
    return OmitDwarfIfHaveCompactUnwind;
  }

  bool getCommDirectiveSupportsAlignment() const {
    return CommDirectiveSupportsAlignment;
  }

  unsigned getFDEEncoding() const { return FDECFIEncoding; }

  unsigned getCompactUnwindDwarfEHFrameOnly() const {
    return CompactUnwindDwarfEHFrameOnly;
  }

  MCSection *getTextSection() const { return TextSection; }
  MCSection *getDataSection() const { return DataSection; }
  MCSection *getBSSSection() const { return BSSSection; }
  MCSection *getReadOnlySection() const { return ReadOnlySection; }
  MCSection *getCompactUnwindSection() const { return CompactUnwindSection; }
  MCSection *getDwarfAbbrevSection() const { return DwarfAbbrevSection; }
  MCSection *getDwarfInfoSection() const { return DwarfInfoSection; }
  MCSection *getDwarfInfoSection(uint64_t Hash) const {
    return getDwarfComdatSection(".debug_info", Hash);
  }
  MCSection *getDwarfLineSection() const { return DwarfLineSection; }
  MCSection *getDwarfLineStrSection() const { return DwarfLineStrSection; }
  MCSection *getDwarfFrameSection() const { return DwarfFrameSection; }
  MCSection *getDwarfPubNamesSection() const { return DwarfPubNamesSection; }
  MCSection *getDwarfPubTypesSection() const { return DwarfPubTypesSection; }
  MCSection *getDwarfGnuPubNamesSection() const {
    return DwarfGnuPubNamesSection;
  }
  MCSection *getDwarfGnuPubTypesSection() const {
    return DwarfGnuPubTypesSection;
  }
  const MCSection *getDwarfDebugInlineSection() const {
    return DwarfDebugInlineSection;
  }
  MCSection *getDwarfStrSection() const { return DwarfStrSection; }
  MCSection *getDwarfLocSection() const { return DwarfLocSection; }
  MCSection *getDwarfARangesSection() const { return DwarfARangesSection; }
  MCSection *getDwarfRangesSection() const { return DwarfRangesSection; }
  MCSection *getDwarfRnglistsSection() const { return DwarfRnglistsSection; }
  MCSection *getDwarfLoclistsSection() const { return DwarfLoclistsSection; }
  MCSection *getDwarfMacinfoSection() const { return DwarfMacinfoSection; }
  MCSection *getDwarfMacroSection() const { return DwarfMacroSection; }

  MCSection *getDwarfDebugNamesSection() const {
    return DwarfDebugNamesSection;
  }
  MCSection *getDwarfAccelNamesSection() const {
    return DwarfAccelNamesSection;
  }
  MCSection *getDwarfAccelObjCSection() const { return DwarfAccelObjCSection; }
  MCSection *getDwarfAccelNamespaceSection() const {
    return DwarfAccelNamespaceSection;
  }
  MCSection *getDwarfAccelTypesSection() const {
    return DwarfAccelTypesSection;
  }
  MCSection *getDwarfInfoDWOSection() const { return DwarfInfoDWOSection; }
  MCSection *getDwarfTypesSection(uint64_t Hash) const {
    return getDwarfComdatSection(".debug_types", Hash);
  }
  MCSection *getDwarfTypesDWOSection() const { return DwarfTypesDWOSection; }
  MCSection *getDwarfAbbrevDWOSection() const { return DwarfAbbrevDWOSection; }
  MCSection *getDwarfStrDWOSection() const { return DwarfStrDWOSection; }
  MCSection *getDwarfLineDWOSection() const { return DwarfLineDWOSection; }
  MCSection *getDwarfLocDWOSection() const { return DwarfLocDWOSection; }
  MCSection *getDwarfStrOffDWOSection() const { return DwarfStrOffDWOSection; }
  MCSection *getDwarfStrOffSection() const { return DwarfStrOffSection; }
  MCSection *getDwarfAddrSection() const { return DwarfAddrSection; }
  MCSection *getDwarfRnglistsDWOSection() const {
    return DwarfRnglistsDWOSection;
  }
  MCSection *getDwarfLoclistsDWOSection() const {
    return DwarfLoclistsDWOSection;
  }
  MCSection *getDwarfMacroDWOSection() const { return DwarfMacroDWOSection; }
  MCSection *getDwarfMacinfoDWOSection() const {
    return DwarfMacinfoDWOSection;
  }
  MCSection *getDwarfCUIndexSection() const { return DwarfCUIndexSection; }
  MCSection *getDwarfTUIndexSection() const { return DwarfTUIndexSection; }
  MCSection *getDwarfSwiftASTSection() const { return DwarfSwiftASTSection; }

  MCSection *getCOFFDebugSymbolsSection() const {
    return COFFDebugSymbolsSection;
  }
  MCSection *getCOFFDebugTypesSection() const {
    return COFFDebugTypesSection;
  }
  MCSection *getCOFFGlobalTypeHashesSection() const {
    return COFFGlobalTypeHashesSection;
  }

  MCSection *getTLSExtraDataSection() const { return TLSExtraDataSection; }
  const MCSection *getTLSDataSection() const { return TLSDataSection; }
  MCSection *getTLSBSSSection() const { return TLSBSSSection; }

  MCSection *getStackMapSection() const { return StackMapSection; }
  MCSection *getFaultMapSection() const { return FaultMapSection; }
  MCSection *getRemarksSection() const { return RemarksSection; }

  MCSection *getStackSizesSection(const MCSection &TextSec) const;

  MCSection *getBBAddrMapSection(const MCSection &TextSec) const; 
 
  MCSection *getPseudoProbeSection(const MCSection *TextSec) const; 
 
  MCSection *getPseudoProbeDescSection(StringRef FuncName) const; 
 
  // ELF specific sections.
  MCSection *getDataRelROSection() const { return DataRelROSection; }
  const MCSection *getMergeableConst4Section() const {
    return MergeableConst4Section;
  }
  const MCSection *getMergeableConst8Section() const {
    return MergeableConst8Section;
  }
  const MCSection *getMergeableConst16Section() const {
    return MergeableConst16Section;
  }
  const MCSection *getMergeableConst32Section() const {
    return MergeableConst32Section;
  }

  // MachO specific sections.
  const MCSection *getTLSTLVSection() const { return TLSTLVSection; }
  const MCSection *getTLSThreadInitSection() const {
    return TLSThreadInitSection;
  }
  const MCSection *getCStringSection() const { return CStringSection; }
  const MCSection *getUStringSection() const { return UStringSection; }
  MCSection *getTextCoalSection() const { return TextCoalSection; }
  const MCSection *getConstTextCoalSection() const {
    return ConstTextCoalSection;
  }
  const MCSection *getConstDataSection() const { return ConstDataSection; }
  const MCSection *getDataCoalSection() const { return DataCoalSection; }
  const MCSection *getConstDataCoalSection() const {
    return ConstDataCoalSection;
  }
  const MCSection *getDataCommonSection() const { return DataCommonSection; }
  MCSection *getDataBSSSection() const { return DataBSSSection; }
  const MCSection *getFourByteConstantSection() const {
    return FourByteConstantSection;
  }
  const MCSection *getEightByteConstantSection() const {
    return EightByteConstantSection;
  }
  const MCSection *getSixteenByteConstantSection() const {
    return SixteenByteConstantSection;
  }
  MCSection *getLazySymbolPointerSection() const {
    return LazySymbolPointerSection;
  }
  MCSection *getNonLazySymbolPointerSection() const {
    return NonLazySymbolPointerSection;
  }
  MCSection *getThreadLocalPointerSection() const {
    return ThreadLocalPointerSection;
  }

  // COFF specific sections.
  MCSection *getDrectveSection() const { return DrectveSection; }
  MCSection *getPDataSection() const { return PDataSection; }
  MCSection *getXDataSection() const { return XDataSection; }
  MCSection *getSXDataSection() const { return SXDataSection; }
  MCSection *getGFIDsSection() const { return GFIDsSection; }
  MCSection *getGIATsSection() const { return GIATsSection; } 
  MCSection *getGLJMPSection() const { return GLJMPSection; }

  // XCOFF specific sections
  MCSection *getTOCBaseSection() const { return TOCBaseSection; }

  MCSection *getEHFrameSection() const { return EHFrameSection; } 

  enum Environment { IsMachO, IsELF, IsCOFF, IsWasm, IsXCOFF };
  Environment getObjectFileType() const { return Env; }

  bool isPositionIndependent() const { return PositionIndependent; }

private:
  Environment Env;
  bool PositionIndependent = false;
  MCContext *Ctx = nullptr;
  Triple TT;
  VersionTuple SDKVersion;

  void initMachOMCObjectFileInfo(const Triple &T);
  void initELFMCObjectFileInfo(const Triple &T, bool Large);
  void initCOFFMCObjectFileInfo(const Triple &T);
  void initWasmMCObjectFileInfo(const Triple &T);
  void initXCOFFMCObjectFileInfo(const Triple &T);
  MCSection *getDwarfComdatSection(const char *Name, uint64_t Hash) const;

public:
  const Triple &getTargetTriple() const { return TT; }

  void setSDKVersion(const VersionTuple &TheSDKVersion) {
    SDKVersion = TheSDKVersion;
  }

  const VersionTuple &getSDKVersion() const { return SDKVersion; }
};

} // end namespace llvm

#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
