# Generated by devtools/yamaker.

LIBRARY()

OWNER(
    orivej
    g:cpp-contrib
)

LICENSE(Apache-2.0 WITH LLVM-exception)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm12 
    contrib/libs/llvm12/include 
    contrib/libs/llvm12/lib/Analysis 
    contrib/libs/llvm12/lib/Bitcode/Reader 
    contrib/libs/llvm12/lib/Bitcode/Writer 
    contrib/libs/llvm12/lib/Frontend/OpenMP 
    contrib/libs/llvm12/lib/IR 
    contrib/libs/llvm12/lib/IRReader 
    contrib/libs/llvm12/lib/Linker 
    contrib/libs/llvm12/lib/Object 
    contrib/libs/llvm12/lib/ProfileData 
    contrib/libs/llvm12/lib/Support 
    contrib/libs/llvm12/lib/Transforms/AggressiveInstCombine 
    contrib/libs/llvm12/lib/Transforms/InstCombine 
    contrib/libs/llvm12/lib/Transforms/Instrumentation 
    contrib/libs/llvm12/lib/Transforms/Scalar 
    contrib/libs/llvm12/lib/Transforms/Utils 
    contrib/libs/llvm12/lib/Transforms/Vectorize 
)

ADDINCL(
    contrib/libs/llvm12/lib/Transforms/IPO
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    AlwaysInliner.cpp
    Annotation2Metadata.cpp 
    ArgumentPromotion.cpp
    Attributor.cpp
    AttributorAttributes.cpp
    BarrierNoopPass.cpp
    BlockExtractor.cpp
    CalledValuePropagation.cpp
    ConstantMerge.cpp
    CrossDSOCFI.cpp
    DeadArgumentElimination.cpp
    ElimAvailExtern.cpp
    ExtractGV.cpp
    ForceFunctionAttrs.cpp
    FunctionAttrs.cpp
    FunctionImport.cpp
    GlobalDCE.cpp
    GlobalOpt.cpp
    GlobalSplit.cpp
    HotColdSplitting.cpp
    IPO.cpp
    IROutliner.cpp 
    InferFunctionAttrs.cpp
    InlineSimple.cpp
    Inliner.cpp
    Internalize.cpp
    LoopExtractor.cpp
    LowerTypeTests.cpp
    MergeFunctions.cpp
    OpenMPOpt.cpp
    PartialInlining.cpp
    PassManagerBuilder.cpp
    PruneEH.cpp
    SCCP.cpp
    SampleContextTracker.cpp 
    SampleProfile.cpp
    SampleProfileProbe.cpp 
    StripDeadPrototypes.cpp
    StripSymbols.cpp
    SyntheticCountsPropagation.cpp
    ThinLTOBitcodeWriter.cpp
    WholeProgramDevirt.cpp
)

END()
