# Generated by devtools/yamaker.

LIBRARY()

OWNER(
    orivej
    g:cpp-contrib
)

LICENSE(
    Apache-2.0 WITH LLVM-exception AND
    NCSA
)

LICENSE_TEXTS(.yandex_meta/licenses.list.txt)

PEERDIR(
    contrib/libs/llvm12 
    contrib/libs/llvm12/include 
    contrib/libs/llvm12/lib/BinaryFormat 
    contrib/libs/llvm12/lib/IR 
    contrib/libs/llvm12/lib/Object 
    contrib/libs/llvm12/lib/ProfileData 
    contrib/libs/llvm12/lib/Support 
)

ADDINCL(
    contrib/libs/llvm12/lib/Analysis
)

NO_COMPILER_WARNINGS()

NO_UTIL()

SRCS(
    AliasAnalysis.cpp
    AliasAnalysisEvaluator.cpp
    AliasAnalysisSummary.cpp
    AliasSetTracker.cpp
    Analysis.cpp
    AssumeBundleQueries.cpp
    AssumptionCache.cpp
    BasicAliasAnalysis.cpp
    BlockFrequencyInfo.cpp
    BlockFrequencyInfoImpl.cpp
    BranchProbabilityInfo.cpp
    CFG.cpp
    CFGPrinter.cpp
    CFLAndersAliasAnalysis.cpp
    CFLSteensAliasAnalysis.cpp
    CGSCCPassManager.cpp
    CallGraph.cpp
    CallGraphSCCPass.cpp
    CallPrinter.cpp
    CaptureTracking.cpp
    CmpInstAnalysis.cpp
    CodeMetrics.cpp
    ConstantFolding.cpp
    ConstraintSystem.cpp 
    CostModel.cpp
    DDG.cpp
    DDGPrinter.cpp 
    Delinearization.cpp
    DemandedBits.cpp
    DependenceAnalysis.cpp
    DependenceGraphBuilder.cpp
    DevelopmentModeInlineAdvisor.cpp 
    DivergenceAnalysis.cpp
    DomPrinter.cpp
    DomTreeUpdater.cpp
    DominanceFrontier.cpp
    EHPersonalities.cpp
    FunctionPropertiesAnalysis.cpp 
    GlobalsModRef.cpp
    GuardUtils.cpp
    HeatUtils.cpp
    IRSimilarityIdentifier.cpp 
    IVDescriptors.cpp
    IVUsers.cpp
    ImportedFunctionsInliningStatistics.cpp 
    IndirectCallPromotionAnalysis.cpp
    InlineAdvisor.cpp
    InlineCost.cpp
    InlineSizeEstimatorAnalysis.cpp
    InstCount.cpp
    InstructionPrecedenceTracking.cpp
    InstructionSimplify.cpp
    Interval.cpp
    IntervalPartition.cpp
    LazyBlockFrequencyInfo.cpp
    LazyBranchProbabilityInfo.cpp
    LazyCallGraph.cpp
    LazyValueInfo.cpp
    LegacyDivergenceAnalysis.cpp
    Lint.cpp
    Loads.cpp
    LoopAccessAnalysis.cpp
    LoopAnalysisManager.cpp
    LoopCacheAnalysis.cpp
    LoopInfo.cpp
    LoopNestAnalysis.cpp
    LoopPass.cpp
    LoopUnrollAnalyzer.cpp
    MLInlineAdvisor.cpp 
    MemDepPrinter.cpp
    MemDerefPrinter.cpp
    MemoryBuiltins.cpp
    MemoryDependenceAnalysis.cpp
    MemoryLocation.cpp
    MemorySSA.cpp
    MemorySSAUpdater.cpp
    ModuleDebugInfoPrinter.cpp
    ModuleSummaryAnalysis.cpp
    MustExecute.cpp
    ObjCARCAliasAnalysis.cpp
    ObjCARCAnalysisUtils.cpp
    ObjCARCInstKind.cpp
    OptimizationRemarkEmitter.cpp
    PHITransAddr.cpp
    PhiValues.cpp
    PostDominators.cpp
    ProfileSummaryInfo.cpp
    PtrUseVisitor.cpp
    RegionInfo.cpp
    RegionPass.cpp
    RegionPrinter.cpp
    ReleaseModeModelRunner.cpp 
    ReplayInlineAdvisor.cpp 
    ScalarEvolution.cpp
    ScalarEvolutionAliasAnalysis.cpp
    ScalarEvolutionDivision.cpp
    ScalarEvolutionNormalization.cpp
    ScopedNoAliasAA.cpp
    StackLifetime.cpp
    StackSafetyAnalysis.cpp
    SyncDependenceAnalysis.cpp
    SyntheticCountsUtils.cpp
    TFUtils.cpp 
    TargetLibraryInfo.cpp
    TargetTransformInfo.cpp
    Trace.cpp
    TypeBasedAliasAnalysis.cpp
    TypeMetadataUtils.cpp
    VFABIDemangling.cpp
    ValueLattice.cpp
    ValueLatticeUtils.cpp
    ValueTracking.cpp
    VectorUtils.cpp
)

END()
