#include <iomanip>
#include "../include/reportsystem.h"

__internalReport::__reportSystem debug = __internalReport::__reportSystem();

// Out of line initializations.

VerboseLevel Level = VerboseLevel::ERROR;

const std::map<InfoCode, const char *> __internalReport::__reportSystem::InfoMessages = 
{
    { InfoCode::CuttingSequence, 
        "Cutting sequence \"[ŧag]\" at first appearance of stop codon \"[tag]\" (residue \"[tag]\") at position [tag] (length: [tag] \")" }, 
        
    { InfoCode::WindowSizeCompareset, 
        "Try with specific comparison file window value. Parameter -cw" }, 
};
    
const std::map<WarningCode, const char *> __internalReport::__reportSystem::WarningMessages = 
{
    { WarningCode::RemovingOnlyGapsColumn, 
        "Removing column '[tag]' composed only by gaps" }, 
        
    { WarningCode::KeepingOnlyGapsColumn, 
        "Keeping column '[tag]' composed only by gaps" }, 
        
    { WarningCode::SequenceWillBeCutted, 
        "Sequence \"[tag]\" will be cutted at position [tag] (length:[tag])" }, 
        
    { WarningCode::IncludingIndeterminationSymbols,
        "Sequence \"[tag]\" has some indetermination symbols 'X' at the end of sequence. They will be included in the final newAlignment." },
        
    { WarningCode::LessNucleotidesThanExpected,
        "Sequence \"[tag]\" has less nucleotides ([tag]) than expected ([tag]). It will be added N's to complete the sequence" },
        
    { WarningCode::HeaderWillBeCut,
        "Original sequence header will be cutted by 10 characters" }
        
};
    
const std::map<ErrorCode, const char *> __internalReport::__reportSystem::ErrorMessages = 
{
    { ErrorCode::AlignmentNotLoaded, 
        "Alignment not loaded: \" [tag] \" Check the file's content" }, 
        
    { ErrorCode::NoFormatsSpecified, 
        "You must specify at least one format after the '-formats' argument" }, 
        
    { ErrorCode::AlternativeMatrixNotRecognized, 
        "Alternative matrix \" [tag] \" not recognized" }, 
        
    { ErrorCode::ReferenceFileNotLoaded, 
        "Reference file \" [tag] \" not loaded" }, 
        
    { ErrorCode::GapThresholdOutOfRange, 
        "The gap threshold value should be between 0 and 1" }, 
        
    { ErrorCode::GapThresholdNotRecognized, 
        "The gap threshold value should be a positive real number between 0 and 1" }, 
        
    { ErrorCode::SimilarityThresholdOutOfRange, 
        "The similarity threshold value should be between 0 and 1" }, 
        
    { ErrorCode::SimilarityThresholdNotRecognized, 
        "The similarity threshold value should be a positive real number between 0 and 1" }, 
        
    { ErrorCode::ConsistencyThresholdOutOfRange, 
        "The consistency threshold value should be between 0 and 1" }, 
        
    { ErrorCode::ConsistencyThresholdNotRecognized, 
        "The consistency threshold value should be a positive real number between 0 and 1" }, 
        
    { ErrorCode::ConservationThresholdOutOfRange, 
        "The conservation threshold value should be between 0 and 100" }, 
        
    { ErrorCode::ConservationThresholdNotRecognized, 
        "The conservation threshold value should be a positive real number between 0 and 100" }, 
        
    { ErrorCode::ResidueOverlapOutOfRange, 
        "The residue overlap value should be between 0 and 100" }, 
        
    { ErrorCode::ResidueOverlapNotRecognized, 
        "The residue overlap value should be a positive real number between 0 and 100" }, 
        
    { ErrorCode::SequencesOverlapOutOfRange, 
        "The sequences overlap value should be between 0 and 100" }, 
        
    { ErrorCode::SequencesOverlapNotRecognized, 
        "The sequences overlap value should be a positive real number between 0 and 100" }, 
        
    { ErrorCode::MaxIdentityOutOfRange, 
        "The max identity value should be between 0 and 1" }, 
        
    { ErrorCode::MaxIdentityNotRecognized, 
        "The max identity value should be a positive real number between 0 and 1" }, 
        
    { ErrorCode::ClustersValueOutOfRange, 
        "The clusters value should be greater than 0" }, 
        
    { ErrorCode::ClustersValueNotRecognized, 
        "The clusters value should be a positive integer number greater than 0" }, 
        
    { ErrorCode::WindowValueOutOfRange, 
        "The window value should be greater than 0" }, 
        
    { ErrorCode::WindowValueNotRecognized, 
        "The window value should be a positive integer number greater than 0" }, 
        
    { ErrorCode::SelectSeqsNotRecognized, 
        "Could not parse the -selectseqs ranges" }, 
        
    { ErrorCode::SelectColsNotRecognized, 
        "Could not parse the -selectres ranges" }, 
        
    { ErrorCode::GapWindowValueOutOfRange, 
        "The gap window value should be greater than 0" }, 
        
    { ErrorCode::GapWindowValueNotRecognized, 
        "The gap window value should be a positive integer number grater than 0" }, 
        
    { ErrorCode::SimilarityWindowValueOutOfRange, 
        "The similarity window value should be greater than 0" }, 
        
    { ErrorCode::SimilarityWindowValueNotRecognized, 
        "The similarity window value should be a positive integer number grater than 0" }, 
        
    { ErrorCode::ConsistencyThresholdOutOfRange, 
        "The consistency window value should be greater than 0" }, 
        
    { ErrorCode::ConsistencyThresholdNotRecognized, 
        "The consistency window value should be a positive integer number grater than 0" }, 
        
    { ErrorCode::BlockSizeOutOfRange, 
        "The consistency window value should be greater than 0" }, 
        
    { ErrorCode::BlockSizeNotRecognized, 
        "The consistency window value should be a positive integer number grater than 0" }, 
        
    { ErrorCode::InFileComparisonStatistics, 
        "Option -in not valid in combination with file comparision options: -sft || -sfc || -ct" }, 
        
    { ErrorCode::IncompatibleArguments, 
        "Argument [tag] is not compatible with argument [tag]" }, 
        
    { ErrorCode::SelectSeqsResAndThresholdIncompatibilities, 
        "Options -selectCols and -selectSeqs are not allowed in combination with threshold options: (-gt || -st || -ct || -cons)" }, 
    
    { ErrorCode::SelectSeqsResAndAutomathedMethodsIncompatibilities, 
        "Options -selectCols and -selectSeqs are not allowed in combination with automated trimming options: (-nogaps || -noallgaps || -gappyout || -strict || -strictplus || -automated1)." }, 
        
    { ErrorCode::SelectSeqsResAndWindowIncompatibilities, 
        "Options -selectCols and -selectSeqs are not allowed in combination with window options: (-w || -sw || -gw || -wc)" }, 
        
    { ErrorCode::SelectSeqsResAndOverlapIncompatibilites, 
        "Options -selectCols and -selectSeqs are not allowed in combination of overlap options (-resoverlap || -seqoverlap)" }, 
        
    { ErrorCode::OnlyOneSequencesSelectionMethodAllowed, 
        "Only one method to chose sequences can be applied: (-selectseqs || -clusters || -maxIdentity" }, 
        
    { ErrorCode::CombinationAmongTrimmingMethods, 
        "Only one trimming method can be used at the same time, either automatic or manual" }, 
        
    { ErrorCode::AutomathicMethodAndBlock, 
        "Combination between automatic methods and -block options is not allowed" }, 
        
    { ErrorCode::WindowAndArgumentIncompatibilities, 
        "Combination between general and specific windows is not allowed" }, 
        
    { ErrorCode::CombinationAmongThresholdsMethods, 
        "Combination among thresholds are not allowed" }, 
        
    { ErrorCode::GeneralAndSpecificWindows, 
        "General window (-w) is not compatible with specific window options: (-cw, -gw, -sw)" }, 
    
    { ErrorCode::StatisticsArgumentIncompatibilities, 
        "Parameter [tag] is not valid when statistics' parameters are defined" }, 
        
    { ErrorCode::TrimmingMethodNeeded, 
        "Parameter [tag] can only be used with either an automatic or a manual method" }, 
        
    { ErrorCode::ForceFileWithoutCompareDataset, 
        "You can not force the alignment selection without setting an alignment dataset to compare against" }, 
        
    { ErrorCode::BacktranslationWithoutMainAlignment, 
        "You need to specify an input alignment (-in or -forcefile) to use a Coding Sequence File (-backtranslation) to apply the back translation method" }, 
        
    { ErrorCode::NotAligned, 
        "The sequences in the input alignment [tag] should be aligned in order to use any trimming method or statistics" }, 
        
    { ErrorCode::MatrixGivenWithNoMethodToUseIt, 
        "The Similarity Matrix can only be used with methods that use this matrix" }, 
        
    { ErrorCode::SameNameOutput, 
        "The [tag] and [tag] files can't be the same" }, 
        
    { ErrorCode::SequenceAndResiduesOverlapMutuallyNeeded, 
        "Sequence and residues overlap values are mutually needed. You only specified [tag]" }, 
        
    { ErrorCode::OutFileNeededWhenPrintingStatistics, 
        "Out file must be specified in order to print any statistics" }, 
        
    { ErrorCode::AlignmentTypesNotMatching, 
        "The alignments' datatypes are different. Check your dataset" }, 
        
    { ErrorCode::BlocksizeTooBig, 
        "The block size value is too big. Please, choose another one smaller than residues number / 4. In this case, the limit is: [tag]" }, 
        
    { ErrorCode::ParemeterOnlyOnBacktranslation, 
        "The [tag] parameter can be only set up with backtranslation functionality" }, 
        
    { ErrorCode::ProteinAlignmentMustBeAligned, 
        "The input protein file has to be aligned to carry out the backtranslation process" }, 
                
    { ErrorCode::BacktransAlignIsDNA, 
        "Check your Coding sequences file. It has been detected other kind of biological sequences" }, 
        
    { ErrorCode::ImpossibleToGenerate, 
        "Impossible to generate [tag]" }, 
        
    { ErrorCode::ImpossibleToProcessMatrix, 
        "It's impossible to process the Similarity Matrix" }, 
        
    { ErrorCode::SelectOnlyAccepts, 
        "Option [tag] only accepts ranges from 0 to number of [tag] - 1" }, 
        
    { ErrorCode::MoreClustersThanSequences, 
        "The number of clusters from the Alignment can not be larger than the number of sequences from that alignment" }, 
        
    { ErrorCode::LeftBoundaryBiggerThanRightBoundary, 
        "Check your manually set left '[tag]' and right '[tag]' boundaries'" }, 
        
    { ErrorCode::DifferentNumberOfSequencesInCompareset, 
        "The files to compare do not have the same number of sequences" }, 
    
    { ErrorCode::DifferentSeqsNamesInCompareset, 
        "Sequences names differ in compareset files" }, 

    { ErrorCode::CDScontainsProteinSequences, 
        "Check input CDS file. It seems to content protein residues." }, 
        
    { ErrorCode::SequenceContainsGap, 
        "Sequence \"[tag]\" has, at least, one gap" }, 
        
    { ErrorCode::SequenceNotMultipleOfThree, 
        "Sequence length \"[tag]\" is not multiple of 3 (length: [tag])" }, 
        
    { ErrorCode::SequenceHasStopCodon, 
        "Sequence \"[tag]\" has stop codon \"[tag]\" (residue \"[tag]\") at position [tag] (length: [tag])" }, 
        
    { ErrorCode::SequenceNotPresentInCDS, 
        "Sequence \"[tag]\" is not in CDS file." }, 
        
    { ErrorCode::UnknownCharacter, 
        "The sequence \"[tag]\" has an unknown ([tag]) character." }, 
        
    { ErrorCode::SequencesNotSameSize, 
        "The sequence \"[tag]\" ([tag]) does not have the same number of residues fixed by the alignment ([tag])" }, 
        
    { ErrorCode::IncorrectSymbol, 
        "the symbol '[tag]' is incorrect" }, 
        
    { ErrorCode::UndefinedSymbol, 
        "the symbol '[tag]' accesing the matrix is not defined in this object" }, 
        
    { ErrorCode::ParameterNotFoundOrRepeated, 
        "Parameter \"[tag]\" not valid or repeated." }, 
        
    { ErrorCode::SimilarityMatrixNotCompatibleWindow, 
        "The Similarity Matrix can only be used with general/similarity windows size." }, 
        
    { ErrorCode::PossibleMissmatch, 
        "Possible (\") mismatch for comments" }, 
        
    { ErrorCode::BracketsMissmatchFound, 
        "Brackets (]) mismatch found" }, 
        
    { ErrorCode::UnalignedAlignmentToAlignedFormat, 
        "Sequences are not aligned. Format ([tag]) not compatible with unaligned sequences." }, 
};

void __internalReport::__reportSystem::PrintCodesAndMessages()
{
    switch(Level)
    {
        case VerboseLevel::NONE:
            std::cout << "[VerboseLevel] None" << std::endl;
            break;
        case VerboseLevel::INFO:
            std::cout << "[VerboseLevel] Info" << std::endl;
            break;
        case VerboseLevel::WARNING:
            std::cout << "[VerboseLevel] Warning" << std::endl;
            break;
        case VerboseLevel::ERROR:
            std::cout << "[VerboseLevel] Error" << std::endl;
            break;
    }

    for (int i = 1; i < InfoCode::__MAXINFO ; i++)
    {
        report((InfoCode)i);
    }
    
    for (int i = 1; i < WarningCode::__MAXWARNING ; i++)
    {
        report((WarningCode)i);
    }  
    
    for (int i = 1; i < ErrorCode::__MAXERROR ; i++)
    {
        report((ErrorCode)i);
    }
}

void __internalReport::__reportSystem::report(ErrorCode message, std::string * vars)
{
    if (Level > VerboseLevel::ERROR)
    {
        if (vars != NULL)
            delete [] vars;
    }
    else 
    {
        if (vars == NULL)
        {
            std::cerr << "[ERROR "<< std::setw(3) << std::setfill('0') << message << "] " << ErrorMessages.at(message) << std::endl;
            return;
        }
        
        std::string s(ErrorMessages.at(message));

        std::string FindWord = "[tag]";
        
        int counter = 0;

        std::size_t index;
            while ((index = s.find(FindWord)) != std::string::npos)
                s.replace(index, FindWord.length(), vars[counter++]);

        std::cerr << "[ERROR "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;
        
        delete [] vars;
    }
}

void __internalReport::__reportSystem::report(ErrorCode message, char * vars)
{
    if (Level > VerboseLevel::ERROR) return;
    
    if (vars == NULL)
    {
        std::cerr << "[ERROR "<< std::setw(3) << std::setfill('0') << message << "] " << ErrorMessages.at(message) << std::endl;
        return;
    }
    
    std::string s(ErrorMessages.at(message));

    std::string FindWord = "[tag]";
    
    std::string Vars = vars;

    std::size_t index;
        while ((index = s.find(FindWord)) != std::string::npos)
            s.replace(index, FindWord.length(), Vars);

    std::cerr << "[ERROR "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;

}

void __internalReport::__reportSystem::report(WarningCode message, std::string * vars)
{
    if (Level > VerboseLevel::WARNING)
    {
        if (vars != NULL)
            delete [] vars;
    }
    else 
    {
        if (vars == NULL)
        {
            std::cout << "[WARNING "<< std::setw(3) << std::setfill('0') << message << "] " << WarningMessages.at(message) << std::endl;
            return;
        }
        
        std::string s(WarningMessages.at(message));

        std::string FindWord = "[tag]";
        
        int counter = 0;

        std::size_t index;
            while ((index = s.find(FindWord)) != std::string::npos)
                s.replace(index, FindWord.length(), vars[counter++]);

        std::cout << "[WARNING "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;
        
        delete [] vars;
    }
}

void __internalReport::__reportSystem::report(WarningCode message, char * vars)
{
    if (Level > VerboseLevel::WARNING) return;
    
    if (vars == NULL)
    {
        std::cout << "[WARNING "<< std::setw(3) << std::setfill('0') << message << "] " << WarningMessages.at(message) << std::endl;
        return;
    }
    
    std::string s(WarningMessages.at(message));

    std::string FindWord = "[tag]";
    
    std::string Vars = vars;

    std::size_t index;
        while ((index = s.find(FindWord)) != std::string::npos)
            s.replace(index, FindWord.length(), Vars);

    std::cout << "[WARNING "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;

}

void __internalReport::__reportSystem::report(InfoCode message, std::string * vars)
{
    if (Level > VerboseLevel::INFO)
    {
        if (vars != NULL)
            delete [] vars;
    }
    else 
    {
        if (vars == NULL)
        {
            std::cout << "[INFO "<< std::setw(3) << std::setfill('0') << message << "] " << InfoMessages.at(message) << std::endl;
            return;
        }
        
        std::string s(InfoMessages.at(message));

        std::string FindWord = "[tag]";
        
        int counter = 0;

        std::size_t index;
            while ((index = s.find(FindWord)) != std::string::npos)
                s.replace(index, FindWord.length(), vars[counter++]);

        std::cout << "[INFO "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;
        
        delete [] vars;
    }
}

void __internalReport::__reportSystem::report(InfoCode message, char * vars)
{
    if (Level > VerboseLevel::INFO) return;
    
    if (vars == NULL)
    {
        std::cout << "[INFO "<< std::setw(3) << std::setfill('0') << message << "] " << InfoMessages.at(message) << std::endl;
        return;
    }
    
    std::string s(InfoMessages.at(message));

    std::string FindWord = "[tag]";
    
    std::string Vars = vars;

    std::size_t index;
        while ((index = s.find(FindWord)) != std::string::npos)
            s.replace(index, FindWord.length(), Vars);

    std::cout << "[INFO "<< std::setw(3) << std::setfill('0') << message << "] " << s << std::endl;

}
