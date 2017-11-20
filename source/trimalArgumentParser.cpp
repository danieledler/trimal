//
// Created by bioinfo on 8/06/17.
//

#include <string>

#include "../include/trimalArgumentParser.h"
#include "../include/compareFiles.h"
#include "../include/defines.h"
#include "../include/values.h"
#include "../include/reportsystem.h"
#include "../include/statisticsConservation2.h"

#include "../include/ReadWriteMS/vcf_statish.h"

void menu();
void examples();

trimAlManager::trimAlManager()
{

}

trimAlManager::~trimAlManager()
{
    delete_variables();
}

void trimAlManager::parseArguments(int argc, char *argv[])
{
    if (argc == 1)
    {
        menu();
        exit(0);
    }
    
    verbosity_argument(&argc, argv);

    for(int i = 1; i < argc; i++ )
    {
        if (appearErrors) break;

        info_arguments(&argc, argv, &i);

        if (in_argument(&argc, argv, &i))           continue;
        if (vcf_argument(&argc, argv, &i))          continue;
        if (out_argument(&argc, argv, &i))          continue;
        if (html_out_argument(&argc, argv, &i))     continue;
        if (svg_out_argument(&argc, argv, &i))      continue;
        if (svg_stats_argument(&argc, argv, &i))    continue;
        if (out_format_arguments(&argc, argv, &i))  continue;
        if (matrix_argument(&argc, argv, &i))       continue;

        if (compareset_argument(&argc, argv, &i))   continue;
        if (force_select_argument(&argc, argv, &i)) continue;
        if (back_trans_argument(&argc, argv, &i))   continue;

        if (gap_threshold_argument(&argc, argv, &i))            continue;
        if (similarity_threshold_argument(&argc, argv, &i))     continue;
        if (consistency_threshold_argument(&argc, argv, &i))    continue;

        if (conservation_threshold_argument(&argc, argv, &i))   continue;
        if (select_cols_argument(&argc, argv, &i))              continue;

        if (no_gaps_argument(&argc, argv, &i))              continue;
        if (no_all_gaps_argument(&argc, argv, &i))          continue;

        if (keep_seqs_argument(&argc, argv, &i))            continue;
        if (keep_header_argument(&argc, argv, &i))          continue;

        if (gappy_out_argument(&argc, argv, &i))            continue;
        if (strict_argument(&argc, argv, &i))               continue;
        if (strict_plus_argument(&argc, argv, &i))          continue;
        if (automated1_argument(&argc, argv, &i))           continue;

        if (residue_overlap_argument(&argc, argv, &i))      continue;
        if (sequence_overlap_argument(&argc, argv, &i))     continue;

        if (seqs_select_argument(&argc, argv, &i))          continue;

        if (max_identity_argument(&argc, argv, &i))         continue;
        if (clusters_argument(&argc, argv, &i))             continue;

        if (terminal_only_argument(&argc, argv, &i))        continue;

        if (window_argument(&argc, argv, &i))               continue;
        if (gap_window_argument(&argc, argv, &i))           continue;
        if (similarity_window_argument(&argc, argv, &i))    continue;
        if (consistency_window_argument(&argc, argv, &i))   continue;

        if (block_argument(&argc, argv, &i))                continue;
        if (stats_arguments(&argc, argv, &i))               continue;

        if (complementary_argument(&argc, argv, &i))        continue;

        if (col_numbering_argument(&argc, argv, &i))        continue;
        if (split_by_stop_codon_argument(&argc, argv, &i))  continue;
        if (ignore_stop_codon_argument(&argc, argv, &i))    continue;
        
        if (!strcmp(argv[i], "--verbosity") || !strcmp(argv[i], "-v"))
        {
            i++;
            continue;
        }
        
        debug.report(ErrorCode::ParameterNotFoundOrRepeated, argv[i]);
        appearErrors = true;
        break;
    }
}

inline void trimAlManager::verbosity_argument(int* argc, char* argv[])
{
    for(int i = 1; i < *argc; i++ )
    {
        if (!strcmp(argv[i], "--verbosity") || !strcmp(argv[i], "-v"))
        {
            if ((i + 1) != *argc)
            {
                if (!strcmp(argv[i + 1], "error") || !strcmp(argv[i + 1], "3"))
                {
                    debug.Level = VerboseLevel::ERROR;
                    return;
                }
                if (!strcmp(argv[i + 1], "warning") || !strcmp(argv[i + 1], "2"))
                {
                    debug.Level = VerboseLevel::WARNING;
                    return;
                }
                if (!strcmp(argv[i + 1], "info") || !strcmp(argv[i + 1], "1"))
                {
                    debug.Level = VerboseLevel::INFO;
                    return;
                }
                if (!strcmp(argv[i + 1], "none") || !strcmp(argv[i + 1], "0"))
                {
                    debug.Level = VerboseLevel::NONE;
                    return;
                }
                
                debug.report(ErrorCode::VerboseLevelNotRecognized, new std::string[2] { argv[i + 1] , std::to_string(debug.Level) });
            }
            else 
                debug.report(ErrorCode::NeedToSpecifyVerboseLevel, new std::string[2] { argv[i], std::to_string(debug.Level) });
        }
    }
}

inline void trimAlManager::info_arguments(int *argc, char *argv[], int *i)
{
    if (!strcmp(argv[*i], "-h"))
    {
        menu();
        delete_variables();
        exit(0); // We don't want to continue if we show the help.
    }

    if(!strcmp(argv[*i], "--version"))
    {
        examples();
        exit(0); // We don't want to continue if we show the examples.
    }
    
    if (!strcmp(argv[*i], "-lf") || !strcmp(argv[*i], "--listformats"))
    {
        cout << "Input Formats:  "  << ReadWriteMachine.getInputFormatsAvailable() << endl;
        cout << "Output Formats: "  << ReadWriteMachine.getOutputFormatsAvailable();
        exit(0);
    }
}

inline bool trimAlManager::in_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-in") && ((*i)+1 != *argc) && (infile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        infile = new char[argumentLength + 1];
        strcpy(infile, argv[*i]);
        if ((origAlig = ReadWriteMachine.loadAlignment(infile)) == nullptr)
        {
            debug.report(ErrorCode::AlignmentNotLoaded, infile );
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::vcf_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-vcf") && ((*i)+1 != *argc) && (infile == NULL))
    {
        vcfs = new std::vector<std::string>();
        while (((*i)+1 != *argc))
        {
            ++*i;
            if (argv[*i][0] == '-') { --*i; break; }
            vcfs->push_back(argv[*i]);
//             debug << argv[*i] << std::endl;
        }
        return true;
        
    }
    return false;
}

inline bool trimAlManager::out_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-out")) && ((*i)+1 != *argc) && (outfile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        outfile = new char[argumentLength + 1];
        strcpy(outfile, argv[*i]);
        return true;
    }
    return false;
}

inline bool trimAlManager::html_out_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-htmlout")) && ((*i)+1 != *argc) && (htmlOutFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        htmlOutFile = new char[argumentLength + 1];
        strcpy(htmlOutFile, argv[*i]);
        return true;
    }
    return false;
}

inline bool trimAlManager::svg_out_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-svgout")) && ((*i)+1 != *argc) && (svgOutFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        svgOutFile = new char[argumentLength + 1];
        strcpy(svgOutFile, argv[*i]);
        return true;
    }
    return false;
}

inline bool trimAlManager::svg_stats_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-svgstats")) && ((*i)+1 != *argc) && (svgStatsOutFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        svgStatsOutFile = new char[argumentLength + 1];
        strcpy(svgStatsOutFile, argv[*i]);
        return true;
    }
    return false;
}

bool trimAlManager::out_format_arguments(int *argc, char *argv[], int *i)
{
    {
        if(!strcmp(argv[*i], "-formats") )
        {
            if ((*i + 1) == *argc)
            {
                debug.report(ErrorCode::NoFormatsSpecified);
            }
            while (++(*i) != *argc && argv[*i][0] != '-')
                oformats.push_back(argv[*i]);
            (*i)--;
            return true;
        }
        // Option -clustal 
        else if(!strcmp(argv[*i], "-clustal") )
        {
            oformats.push_back("clustal");
            return true;
        }

        // Option -fasta 
        else if(!strcmp(argv[*i], "-fasta") )
        {
            oformats.push_back("fasta");
            return true;
        }

        // Option -fasta-m10 
        else if(!strcmp(argv[*i], "-fasta_m10") )
        {
            oformats.push_back("fasta");
            ReadWriteMachine.shortNames = true;
            shortNames = true;
            return true;
        }

        // Option -nbrf 
        else if(!strcmp(argv[*i], "-nbrf") )
        {
            oformats.push_back("pir");
            return true;
        }

        // Option -nexus 
        else if(!strcmp(argv[*i], "-nexus") )
        {
            oformats.push_back("nexus");
            return true;
        }

        // Option -mega 
        else if(!strcmp(argv[*i], "-mega") )
        {
            oformats.push_back("mega");
            return true;
        }

        // Option -phylip3.2 
        else if(!strcmp(argv[*i], "-phylip3.2") )
        {
            oformats.push_back("phylip32");
            return true;
        }

        // Option -phylip3.2-m10 
        else if(!strcmp(argv[*i], "-phylip3.2_m10") )
        {
            oformats.push_back("phylip32");
            ReadWriteMachine.shortNames = true;
            shortNames = true;
            return true;
        }

        // Option -phylip 
        else if(!strcmp(argv[*i], "-phylip") )
        {
            oformats.push_back("phylip40");
            return true;
        }

        // Option -phylip-m10 
        else if(!strcmp(argv[*i], "-phylip_m10") )
        {
            oformats.push_back("phylip40");
            ReadWriteMachine.shortNames = true;
            shortNames = true;
            return true;
        }

        // Option -phylip_paml 
        else if(!strcmp(argv[*i], "-phylip_paml") )
        {
            oformats.push_back("phylippaml");
            return true;
        }

        // Option -phylip_paml-m10 
        else if(!strcmp(argv[*i], "-phylip_paml_m10") )
        {
            oformats.push_back("phylippaml");
            ReadWriteMachine.shortNames = true;
            shortNames = true;
            return true;
        }
    }
    return false;

}

inline bool trimAlManager::matrix_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-matrix") && ((*i)+1 != *argc) && (matrixFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        matrixFile = new char[argumentLength + 1];
        strcpy(matrixFile, argv[*i]);
        return true;
    }
    else if(!strcmp(argv[*i], "--alternative_matrix") && ((*i)+1 != *argc) && (alternative_matrix == -1)) {
        i++;
        if (!strcmp(argv[*i], "degenerated_nt_identity"))
            alternative_matrix = 1;
        else {
            debug.report(ErrorCode::AlternativeMatrixNotRecognized, argv[*i]);
            appearErrors = true;
        }
    }
    return false;
}

inline bool trimAlManager::compareset_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-compareset") && ((*i)+1 != *argc) && (compareset == -1))
    {
        compare.open(argv[++*i], ifstream::in);
        if(!compare)
        {
            debug.report(ErrorCode::ReferenceFileNotLoaded, argv[*i]);
            appearErrors = true;
        }

        while(compare.getline(line, 256)) numfiles++;
        compare.close();

        compareset = *i;

        return true;
    }
    return false;
}

inline bool trimAlManager::force_select_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-forceselect") && ((*i)+1 != *argc) && (forceFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        forceFile = new char[argumentLength + 1];
        strcpy(forceFile, argv[*i]);
        if ((origAlig = ReadWriteMachine.loadAlignment(forceFile)) == nullptr)
        {
            debug.report(ErrorCode::AlignmentNotLoaded, forceFile);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::back_trans_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-backtrans") && ((*i)+1 != *argc) && (backtransFile == NULL))
    {
        argumentLength = strlen(argv[++*i]);
        backtransFile = new char[argumentLength + 1];
        strcpy(backtransFile, argv[*i]);

        if ((backtranslationAlig = ReadWriteMachine.loadAlignment(backtransFile)) == nullptr)
        {
            debug.report(ErrorCode::AlignmentNotLoaded, backtransFile);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::gap_threshold_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-gapthreshold") || !strcmp(argv[*i], "-gt")) && ((*i)+1 != *argc) && (gapThreshold == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            gapThreshold = 1. - atof(argv[*i]);
            if((gapThreshold < 0) || (gapThreshold > 1))
            {
                debug.report(ErrorCode::GapThresholdOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::GapThresholdNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::similarity_threshold_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-simthreshold") || !strcmp(argv[*i], "-st")) && ((*i)+1 != *argc) && (similarityThreshold == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            similarityThreshold = atof(argv[*i]);
            if((similarityThreshold < 0) || (similarityThreshold > 1))
            {
                debug.report(ErrorCode::SimilarityThresholdOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::SimilarityThresholdNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::consistency_threshold_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-conthreshold") || !strcmp(argv[*i], "-ct")) && ((*i)+1 != *argc) && (consistencyThreshold == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            consistencyThreshold = atof(argv[*i]);
            if((consistencyThreshold < 0) || (consistencyThreshold > 1))
            {
                debug.report(ErrorCode::ConsistencyThresholdOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::ConsistencyThresholdNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::conservation_threshold_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-cons")) && ((*i)+1 != *argc) && (conservationThreshold == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            conservationThreshold = atof(argv[*i]);
            if((conservationThreshold < 0) || (conservationThreshold > 100))
            {
                debug.report(ErrorCode::ConservationThresholdOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::ConservationThresholdNotRecognized);
            appearErrors = true;
        }

        return true;
    }
    return false;
}

bool trimAlManager::select_cols_argument(int *argc, char *argv[], int *i)
{

    if(!strcmp(argv[*i], "-selectcols") &&
            selectCols == false &&
            (*i+3) < *argc &&
            !strcmp(argv[++(*i)], "{") &&
            !strcmp(argv[(*i)+2], "}"))
    {
        if((delColumns = utils::readNumbers(argv[++(*i)])) == NULL)
        {
            debug.report(ErrorCode::SelectColsNotRecognized);
            appearErrors = true;
        }

        else selectCols = true;
        (*i)++;

        return true;
    }
    return false;
}

inline bool trimAlManager::no_gaps_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-nogaps") && (!nogaps))
    {
        nogaps = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::no_all_gaps_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-noallgaps") && (!noallgaps))
    {
        noallgaps = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::keep_seqs_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-keepseqs") && (!keepSeqs))
    {
        keepSeqs = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::keep_header_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-keepheader") && (!ReadWriteMachine.keepHeader))
    {
        ReadWriteMachine.keepHeader = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::gappy_out_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-gappyout") && (!strict))
    {
        gappyout = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::strict_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-strict") && (!strict))
    {
        strict = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::strict_plus_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-strictplus")) && (!strictplus))
    {
        strictplus = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::automated1_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-automated1")) && (!automated1))
    {
        automated1 = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::residue_overlap_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-resoverlap")) && ((*i)+1 != *argc) && (residuesOverlap == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            residuesOverlap = atof(argv[*i]);
            if((residuesOverlap < 0) || (residuesOverlap > 1))
            {
                debug.report(ErrorCode::ResidueOverlapOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::ResidueOverlapNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::sequence_overlap_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-seqoverlap")) && ((*i)+1 != *argc) && (sequenceOverlap == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            sequenceOverlap = atof(argv[*i]);
            if((sequenceOverlap < 0) || (sequenceOverlap > 100))
            {
                debug.report(ErrorCode::SequencesOverlapOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::SequencesOverlapNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

bool trimAlManager::seqs_select_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-selectseqs")) && (selectSeqs == false) && ((*i+3) < *argc) && (!strcmp(argv[++*i], "{")) && (!strcmp(argv[*i+2], "}")))
    {
        if((delSequences = utils::readNumbers(argv[++*i])) == NULL)
        {
            debug.report(ErrorCode::SelectSeqsNotRecognized);
            appearErrors = true;
        }

        else selectSeqs = true;
        (*i)++;
        return true;
    }
    return false;
}

inline bool trimAlManager::max_identity_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-maxidentity")) && ((*i)+1 != *argc) && (maxIdentity == -1))
    {

        if(utils::isNumber(argv[++*i]))
        {
            maxIdentity = atof(argv[*i]);
            if((maxIdentity < 0) || (maxIdentity > 1))
            {
                debug.report(ErrorCode::MaxIdentityOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::MaxIdentityNotRecognized);
            appearErrors = true;
        }

        return true;
    }
    return false;
}

inline bool trimAlManager::clusters_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-clusters")) && ((*i)+1 != *argc) && (clusters == -1))
    {
        if(utils::isNumber(argv[++*i]))
        {
            clusters = atoi(argv[*i]);
            if(clusters < 1)
            {
                debug.report(ErrorCode::ClustersValueOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::ClustersValueNotRecognized);
            appearErrors = true;
        }

        return true;
    }
    return false;
}

inline bool trimAlManager::terminal_only_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-terminalonly")) && (!terminalOnly))
    {
        terminalOnly = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::window_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-w") && ((*i)+1 != *argc) && (windowSize == -1))
    {
        if(utils::isNumber(argv[*i+1]))
        {
            windowSize = atoi(argv[++*i]);
            if(windowSize <= 0)
            {
                debug.report(ErrorCode::WindowValueOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::WindowValueNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::gap_window_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-gw") && ((*i)+1 != *argc) && (gapWindow == -1))
    {
        if(utils::isNumber(argv[*i+1]))
        {
            gapWindow = atoi(argv[++*i]);
            if(gapWindow <= 0)
            {
                debug.report(ErrorCode::GapWindowValueOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::GapWindowValueNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::similarity_window_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-sw") && ((*i)+1 != *argc) && (similarityWindow == -1))
    {
        if(utils::isNumber(argv[*i+1]))
        {
            similarityWindow = atoi(argv[++*i]);
            if(similarityWindow <= 0)
            {
                debug.report(ErrorCode::SimilarityWindowValueOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::SimilarityWindowValueNotRecognized);
            appearErrors = true;
        }

        return true;
    }
    return false;
}

inline bool trimAlManager::consistency_window_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-cw") && ((*i)+1 != *argc) && (consistencyWindow == -1))
    {
        if(utils::isNumber(argv[*i+1]))
        {
            consistencyWindow = atoi(argv[++*i]);
            if(consistencyWindow <= 0)
            {
                debug.report(ErrorCode::ConsistencyWindowValueOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::ConsistencyWindowValueNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::block_argument(int *argc, char *argv[], int *i)
{
    if(!strcmp(argv[*i], "-block") && ((*i)+1 != *argc) && (blockSize == -1))
    {
        if(utils::isNumber(argv[*i+1]))
        {
            blockSize = atoi(argv[++*i]);
            if(blockSize <= 0)
            {
                debug.report(ErrorCode::BlockSizeOutOfRange);
                appearErrors = true;
            }
        }
        else
        {
            debug.report(ErrorCode::BlockSizeNotRecognized);
            appearErrors = true;
        }
        return true;
    }
    return false;
}

inline bool trimAlManager::stats_arguments(int *argc, char *argv[], int *i)
{
    // This argument means two thing
    if((!strcmp(argv[*i], "-sgc")) )
    {
        if (!sgc)
        {
            sgc = true;
            stats--;
        }

        else return false;
    }
    else if((!strcmp(argv[*i], "-sgt")) )
    {
        if(!sgt)
        {
            sgt = true;
            stats--;
        }

        else return false;
    }
    else if((!strcmp(argv[*i], "-ssc")) )
    {
        if(!scc)
        {
            scc = true;
            stats--;
        }

        else return false;
    }
    else if((!strcmp(argv[*i], "-sst")) )
    {
        if(!sct)
        {
            sct = true;
            stats--;
        }

        else return false;
    }
    else if((!strcmp(argv[*i], "-sident")) && (!sident))
    {
        sident = true;
        stats--;
    }
    else if((!strcmp(argv[*i], "-soverlap")) && (!soverlap))
    {
        soverlap = true;
        stats--;
    }
    else if((!strcmp(argv[*i], "-sfc")) )
    {
        if(!sfc)
        {
            sfc = true;
            stats--;
        }

        else return false;
    }
    else if((!strcmp(argv[*i], "-sft")) )
    {
        if(!sft)
        {
            sft = true;
            stats--;
        }

        else return false;
    }
    else
        return false;
    return true;
}

inline bool trimAlManager::complementary_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-complementary")) && (getComplementary == false))
    {
        getComplementary = true;
        return true;
    }
    return false;

}

inline bool trimAlManager::col_numbering_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-colnumbering")) && (columnNumbering == false))
    {
        columnNumbering = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::split_by_stop_codon_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-splitbystopcodon")) && (splitByStopCodon == false))
    {
        splitByStopCodon = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::ignore_stop_codon_argument(int *argc, char *argv[], int *i)
{
    if((!strcmp(argv[*i], "-ignorestopcodon")) && (ignoreStopCodon == false))
    {
        ignoreStopCodon = true;
        return true;
    }
    return false;
}

bool trimAlManager::processArguments(char* argv[])
{
    if (!appearErrors)
    {
        automatedMethodCount = nogaps + noallgaps + gappyout + strict + strictplus + automated1;

        check_arguments_incompatibilities();

        check_arguments_needs(argv);

    }

    if(appearErrors)
    {
        delete_variables();
        return true;
    }
    return false;
}

inline bool trimAlManager::check_arguments_incompatibilities()
{
    // The incompatibilities are checked only once, so there are arguments with no function to check it's incompatibilities although they have.
    // These are checked within other functions.
    // So if argument A is incompatible with B, A may have this checked in it's incompatibilities function, but B may have no function to check them.

    check_inFile_incompatibilities();
    check_select_cols_and_seqs_incompatibilities();
    check_thresholds_incompatibilities();
    check_automated_methods_incompatibilities();
    check_max_identity_incompatibilities();
    check_clusters_incompatibilities();
    check_windows_incompatibilities();
    check_stats_incompatibilities();
    check_codon_behaviour_incompatibility();
    check_combinations_among_thresholds_incompatibility();

    return appearErrors;
}

inline bool trimAlManager::check_inFile_incompatibilities()
{
    if (infile != NULL)
    {
        if((sfc) || (sft) || (consistencyThreshold != -1))
        {
            debug.report(ErrorCode::InFileComparisonStatistics);
            appearErrors = true;
            i++;
        }
        if(compareset != -1)
        {
            debug.report(ErrorCode::IncompatibleArguments, new std::string[2] {"-in", "-compareset"} );
            appearErrors = true;
        }
        if(forceFile != NULL)
        {
            debug.report(ErrorCode::IncompatibleArguments, new std::string[2] {"-in", "-forceselect"} );
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_select_cols_and_seqs_incompatibilities()
{
    if (selectCols || selectSeqs)
    {
        if((gapThreshold != -1) || (conservationThreshold != -1) || (similarityThreshold != -1) || (consistencyThreshold != -1))
        {
            debug.report(ErrorCode::SelectSeqsResAndThresholdIncompatibilities);
            appearErrors = true;
        }

        if(automatedMethodCount)
        {
            debug.report(ErrorCode::SelectSeqsResAndAutomathedMethodsIncompatibilities);
            appearErrors = true;
        }

        if((windowSize != -1) || (gapWindow != -1)|| (similarityWindow != -1) || consistencyWindow != -1)
        {
            debug.report(ErrorCode::SelectSeqsResAndWindowIncompatibilities);
            appearErrors = true;
        }

        if (residuesOverlap != -1 || sequenceOverlap != -1)
        {
            debug.report(ErrorCode::SelectSeqsResAndOverlapIncompatibilites);
            appearErrors = true;
        }

        if((clusters != -1) || (maxIdentity != -1))
        {
            debug.report(ErrorCode::OnlyOneSequencesSelectionMethodAllowed);
            appearErrors = true;
        }

        if (selectCols)
            if(blockSize != -1)
            {
                debug.report(ErrorCode::IncompatibleArguments, new std::string[2] {"-selectcols", "-block"});
                appearErrors = true;
            }
    }
    return appearErrors;
}

inline bool trimAlManager::check_thresholds_incompatibilities()
{
    //TODO consistencyThreshold was not present on this Incompatibilities. Is this correct?
    if((gapThreshold != -1) || (conservationThreshold != -1) || (similarityThreshold != -1) || (consistencyThreshold != -1) ) 
    {
        if(automatedMethodCount)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }

        if (conservationThreshold != -1) // TODO is this unique? or a threshold generality?
        {
            if(blockSize != -1)
            {
                debug.report(ErrorCode::IncompatibleArguments, new std::string[2] {"-conthreshold", "-block"});
                appearErrors = true;
            }
        }

        if (maxIdentity != -1)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }

        if (clusters != -1)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_automated_methods_incompatibilities()
{
    if (automatedMethodCount)
    {
        if((windowSize != -1) || (gapWindow != -1) || (similarityWindow != -1))
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }

        if(blockSize != -1)
        {
            debug.report(ErrorCode::AutomathicMethodAndBlock);
            appearErrors = true;
        }

        if (clusters != -1)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }

        if (maxIdentity != -1)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }

        if (automatedMethodCount > 1)
        {
            debug.report(ErrorCode::CombinationAmongTrimmingMethods);
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_max_identity_incompatibilities()
{
    if (maxIdentity != -1)
    {
        if((windowSize != -1) || (gapWindow != -1) || (similarityWindow != -1) || (consistencyWindow != -1))
        {
            debug.report(ErrorCode::WindowAndArgumentIncompatibilities, new std::string[1] {"-maxIdentity"});
            appearErrors = true;
        }
        if(clusters != -1)
        {
            debug.report(ErrorCode::OnlyOneSequencesSelectionMethodAllowed);
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_clusters_incompatibilities()
{
    if (clusters != -1)
    {
        if((windowSize != -1) || (gapWindow != -1) || (similarityWindow != -1) || consistencyWindow != -1)
        {
            debug.report(ErrorCode::WindowAndArgumentIncompatibilities, new std::string[1] {"-clusters"});
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_windows_incompatibilities()
{
    if (windowSize != -1)
    {
        if (consistencyWindow != -1 || gapWindow != -1 || similarityWindow != -1)
        {
            debug.report(ErrorCode::GeneralAndSpecificWindows);
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_stats_incompatibilities()
{
    if(stats < 0)
    {
        if (columnNumbering)
        {
            debug.report(ErrorCode::StatisticsArgumentIncompatibilities, new string[1] {"-colnumbering"});
            appearErrors = true;
        }
    }
    return appearErrors;
}

inline bool trimAlManager::check_arguments_needs(char* argv[])
{
    check_force_selection();
    check_input_file_with_coding_sequences_argument();
    check_file_aligned();
    check_similarity_matrix();
    check_outputs_coincidence();
    check_col_numbering();
    check_residue_and_sequence_overlap();
    check_output_relevance();
    check_output_file_with_statistics();
    check_automated_manual_incompatibilities();
    check_multiple_files_comparison(argv);
    check_block_size();
    check_backtranslations();
    check_coding_sequences_type();
    check_and_prepare_coding_sequence();
    check_backtranslation_infile_names_corresponde();
    check_cw_argument();
    check_output_format();
    return appearErrors;
}

inline bool trimAlManager::check_codon_behaviour_incompatibility()
{
    if((!appearErrors) && (ignoreStopCodon) && (splitByStopCodon))
    {
        debug.report(ErrorCode::IncompatibleArguments, new std::string[2] {"-ignorestopcodon", "-splitbystopcodon"});
        appearErrors = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::check_combinations_among_thresholds_incompatibility() // TODO is this ok?
{
    if((consistencyThreshold != -1) && (conservationThreshold != -1) && (!appearErrors))
    {

        if((gapThreshold != -1) || (similarityThreshold != -1))
        {
            debug.report(ErrorCode::CombinationAmongThresholdsMethods);
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_automated_manual_incompatibilities()
{
    if((getComplementary) && (!appearErrors))
        if(!automatedMethodCount && // Are we not using an automated method?
                (gapThreshold == -1) && (conservationThreshold == -1) && (similarityThreshold == -1) && // Neither a threshold method.
                (!selectCols) && (!selectSeqs) && (residuesOverlap == -1) && (sequenceOverlap == -1) && // Neither a sequence and residues semimanual selection methods
                (maxIdentity == -1) && (clusters == -1)) // Or complex selection of sequences.
        {
            debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] { "-complementary" });
            appearErrors = true;
            return true;
        }

    if((terminalOnly) && (!appearErrors))
        if(!automatedMethodCount && // Are we not using an automated method?
                (gapThreshold == -1) && (conservationThreshold == -1) && (similarityThreshold == -1) && // Neither a threshold method.
                (!selectCols) && (!selectSeqs) && (residuesOverlap == -1) && (sequenceOverlap == -1) && // Neither a sequence and residues semimanual selection methods
                (maxIdentity == -1) && (clusters == -1)) // Or complex selection of sequences.
        {
            debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] { "-terminalonly" });
            appearErrors = true;
            return true;
        }
    return false;
}

inline bool trimAlManager::check_force_selection()
{
    if (!appearErrors)
    {
        if((compareset == -1) && (forceFile != NULL))
        {
            debug.report(ErrorCode::ForceFileWithoutCompareDataset);
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_input_file_with_coding_sequences_argument()
{
    if((!appearErrors) && (infile == NULL) && (compareset == -1) && (forceFile == NULL) && (backtransFile != NULL))
    {
        debug.report(ErrorCode::BacktranslationWithoutMainAlignment);
        appearErrors = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::check_file_aligned()
{
    if((!appearErrors) && (infile != NULL))
    {

        if((automatedMethodCount || // Are we using an automated method?
                (gapThreshold != -1) || (conservationThreshold != -1) || (similarityThreshold != -1) || // Are we using any threshold?
                (selectCols) || (selectSeqs) || (residuesOverlap != -1) || (sequenceOverlap != -1) || // Are we selecting columns or sequences?
                (stats < 0))  // Are we asking for any stats?
                &&
                (!origAlig -> isFileAligned())) // Then we need the alignment to be aligned;
        {
            debug.report(ErrorCode::NotAligned, new std::string[1] { infile });
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_similarity_matrix()
{
    if((matrixFile != NULL) && (!appearErrors))
    {
        if((!strict) && (!strictplus) && (!automated1) && (similarityThreshold == -1) && (!scc) && (!sct))
        {
            debug.report(ErrorCode::MatrixGivenWithNoMethodToUseIt);
            appearErrors = true;
            return true;
        }

        // TODO this is an incompatibility.
        if((gapWindow != -1) || ((compareset == -1) && (consistencyWindow != -1)))
        {
            debug.report(ErrorCode::SimilarityMatrixNotCompatibleWindow);
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_outputs_coincidence()
{

    std::array<char*,8> outFiles
    {{
            htmlOutFile,
            outfile,
            svgOutFile,
            svgStatsOutFile
        }
    };
    std::array<string,8> outFilesNames
    {{
            "html (-htmlout)",
            "output (-out)",
            "svg (-svgout)",
            "svg stats (-svgstats)"
        }
    };

    for (int i = 0, x = 0; i < outFiles.size(); i++)
    {
        if (outFiles.at(i) != NULL)
            for (x = i + 1; x < outFiles.size(); x++)
            {
                if (outFiles.at(x) != NULL)
                    if (!strcmp(outFiles.at(i), outFiles.at(x)))
                    {
                        debug.report(ErrorCode::SameNameOutput, new std::string[2] { outFilesNames.at(i), outFilesNames.at(x) });
                        appearErrors = true;
                    }
            }
    }

    return false;
}

inline bool trimAlManager::check_col_numbering()
{
    if((columnNumbering) && (!appearErrors))
    {
        if( (!automatedMethodCount) && // Are we not using any automated method?
                (gapThreshold == -1) && (conservationThreshold == -1) && (similarityThreshold == -1) && (consistencyThreshold == -1) && // Are we not using any threshold?
                (!selectCols) && (!selectSeqs)) // Neither selecting any column or sequence?
        {
            debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] {"-colnumbering"} );
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_residue_and_sequence_overlap()
{
    if(!appearErrors)
    {
        if((residuesOverlap != -1) && (sequenceOverlap == -1))
        {
            debug.report(ErrorCode::SequenceAndResiduesOverlapMutuallyNeeded, new string[1] {"residues overlap"} );
            appearErrors = true;
            return true;
        }

        else if((residuesOverlap == -1) && (sequenceOverlap != -1))
        {
            debug.report(ErrorCode::SequenceAndResiduesOverlapMutuallyNeeded, new string[1] {"sequences overlap"} );
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_output_relevance()
{
    if(((htmlOutFile != NULL) || (svgOutFile != NULL) || (svgStatsOutFile != NULL)) && (!appearErrors))
    {
        if(!automatedMethodCount && // Are we not using any automated method?
                (gapThreshold == -1) && (conservationThreshold == -1) && (similarityThreshold == -1) && (consistencyThreshold == -1) && // Neither using thresholds
                (!selectCols) && (!selectSeqs) && (residuesOverlap == -1) && (sequenceOverlap == -1) && // Neither selecting columns or sequences?
                (maxIdentity == -1) && (clusters == -1)) // Neither using other selecting methods?
        {
            if (htmlOutFile != NULL)
                debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] {"-htmlout"} );
            if (svgOutFile != NULL)
                debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] {"-svgout"} );
            if (svgStatsOutFile != NULL)
                debug.report(ErrorCode::TrimmingMethodNeeded, new std::string[1] {"-svgstats"} );
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_output_file_with_statistics()
{
    // NOTE should this function only take in mind if statistics are requested and outFile is not defined?
    // Either we are using 'some' trimming methods or not, the incompatibility to output multiple info (alignment + stats) is present.
    if((stats < 0) && (!appearErrors))
    {
        stats--; //NOTE Why?

        if(((automatedMethodCount) || // If we are using an automated method
                (gapThreshold != -1) || (conservationThreshold != -1) || (similarityThreshold != -1)) // Or a manual threshold

                && (outfile == NULL)) // We need the outFile to be specified.
        {
            debug.report(ErrorCode::OutFileNeededWhenPrintingStatistics);
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_multiple_files_comparison(char* argv[])
{
    bool hasError = false;
    if((compareset != -1) && (!appearErrors))
    {

        compareAlignmentsArray = new newAlignment*[numfiles];
        filesToCompare = new char*[numfiles];

        compare.open(argv[compareset], ifstream::in);

        for(i = 0; (i < numfiles)  && (!appearErrors); i++)
        {

            for(nline.clear(), compare.read(&c, 1); (c != '\n') && ((!compare.eof())); compare.read(&c, 1))
                nline += c;

            filesToCompare[i] = new char [nline.size() + 1];
            strcpy(filesToCompare[i], nline.c_str());

            if ((compareAlignmentsArray[i] = ReadWriteMachine.loadAlignment(filesToCompare[i])) == NULL)
            {
                debug.report(ErrorCode::AlignmentNotLoaded, new std::string[1] { filesToCompare[i] });
                hasError = true;
            }

            else
            {
                if(!compareAlignmentsArray[i] -> isFileAligned())
                {
                    debug.report(ErrorCode::NotAligned, new std::string[1] {filesToCompare[i]} );
                    hasError = true;
                }
                else
                {
                    compareAlignmentsArray[i] -> SequencesMatrix = new sequencesMatrix(compareAlignmentsArray[i]);

                    if(compareAlignmentsArray[i] -> getNumAminos() > maxAminos)
                        maxAminos = compareAlignmentsArray[i] -> getNumAminos();

                    if((compareAlignmentsArray[i] -> getAlignmentType() != prevType) && (prevType != -1))
                    {
                        debug.report(ErrorCode::AlignmentTypesNotMatching);
                        hasError = true;
                    }
                    else
                        prevType = compareAlignmentsArray[i] -> getAlignmentType();
                }
            }
        }

        if (!appearErrors)
        {
            if(forceFile == NULL)
            {
                compareVect = new float[maxAminos];
                if((stats >= 0) && (outfile != NULL))
                    referFile = compareFiles::algorithm(compareAlignmentsArray, filesToCompare, compareVect, numfiles, true);
                else
                    referFile = compareFiles::algorithm(compareAlignmentsArray, filesToCompare, compareVect, numfiles, false);

                if(windowSize != -1)
                    compareFiles::applyWindow(compareAlignmentsArray[referFile] -> getNumAminos(), windowSize, compareVect);
                else if(consistencyWindow != -1)
                    compareFiles::applyWindow(compareAlignmentsArray[referFile] -> getNumAminos(), consistencyWindow, compareVect);
                
                origAlig = ReadWriteMachine.loadAlignment(filesToCompare[referFile]);
            }
            else
            {
                compareVect = new float[origAlig -> getNumAminos()];
                appearErrors = !compareFiles::forceComparison(compareAlignmentsArray, numfiles, origAlig, compareVect);

                if((windowSize != -1) && (!appearErrors))
                    compareFiles::applyWindow(origAlig -> getNumAminos(), windowSize, compareVect);
                else if((consistencyWindow != -1) && (!appearErrors))
                    compareFiles::applyWindow(origAlig -> getNumAminos(), consistencyWindow, compareVect);
            }
        }

        if (oformats.size() == 0)
        {
            oformats.push_back(ReadWriteMachine.getFileFormatName(filesToCompare[referFile]));
        }
        
        for(i = 0; i < numfiles; i++)
        {
            delete compareAlignmentsArray[i];
            delete filesToCompare[i];
        }
    }
    return hasError;

}
inline bool trimAlManager::check_block_size()
{
    if((!appearErrors) && (origAlig -> getNumAminos() < (blockSize/4)))
    {
        debug.report(ErrorCode::BlocksizeTooBig, new string[1] { std::to_string(origAlig-> getNumAminos() / 4) });
        appearErrors = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::check_backtranslations()
{
    if (!appearErrors)
    {
        if (backtransFile == NULL)
        {
            if(splitByStopCodon)
            {
                debug.report(ErrorCode::ParemeterOnlyOnBacktranslation, new string[1] { "-splitbystopcodon" });
                appearErrors = true;
                return true;
            }
            if(ignoreStopCodon)
            {
                debug.report(ErrorCode::ParemeterOnlyOnBacktranslation, new string[1] { "-ignorestopcodon" });
                appearErrors = true;
                return true;
            }
        }
        else if(!origAlig -> isFileAligned())
        {
            debug.report(ErrorCode::ProteinAlignmentMustBeAligned);
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline bool trimAlManager::check_coding_sequences_type()
{
    if((!appearErrors) && (backtransFile != NULL) && // Is there a backtranslation file?
            (!backtranslationAlig -> getAlignmentType() & SequenceTypes::DNA )) // If so, is it from DNA?
    {
        debug.report(ErrorCode::BacktransAlignIsDNA);
        appearErrors = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::check_and_prepare_coding_sequence()
{
    if((!appearErrors)  && (backtransFile != NULL) &&
            (!backtranslationAlig -> prepareCodingSequence(splitByStopCodon, ignoreStopCodon, origAlig)))
    {
        // Error reporting is made by prepareCodingSequence function.
        appearErrors = true;
        return true;
    }
    return false;
}

inline bool trimAlManager::check_backtranslation_infile_names_corresponde()
{
    //NOTE Maybe we don't need to copy the names and lengths to two new arrays 
    // as we could pass the original names and lengths to the check checkCorrespondence function, 
    // which doesn't modify the pointers passed to them
    if((!appearErrors) && (backtransFile != NULL))
    {
        sequencesNames = new string[backtranslationAlig -> getNumSpecies()];
        sequencesLengths = new int[backtranslationAlig -> getNumSpecies()];
        backtranslationAlig -> getSequences(sequencesNames, sequencesLengths);

        if(origAlig -> checkCorrespondence(sequencesNames, sequencesLengths, backtranslationAlig -> getNumSpecies(), 3) != true)
        {
            appearErrors = true;
            return true;
        }
    }
    return false;
}

inline void trimAlManager::check_cw_argument()
{
    if((!appearErrors) && (windowSize != -1) && (compareset != -1))
        debug.report(InfoCode::WindowSizeCompareset);
}

inline void trimAlManager::check_output_format()
{
    if (oformats.size() == 0 && infile)
    {
        oformats.push_back(ReadWriteMachine.getFileFormatName(infile));
    }
}

int trimAlManager::perform()
{

    if (appearErrors) return -1;

    if(conservationThreshold == -1)
        conservationThreshold  = 0;
    
    if (vcfs != NULL)
    {
        auto XX = ReadWriteMachine.splitAlignmentKeeping(*origAlig);
        
        char replacement = '-';
        
        ngs::readVCF(
        /* Dataset          */ XX, 
        /* VCF Collection   */ *vcfs, 
        /* min Quality      */ 0, 
        /* min Converage    */ 30, 
        /* ignore Filters   */ false,
        /* replacement char */ &replacement
        );
        
        for (int i = 0; i < XX.size(); i++)
        {
            delete origAlig;
            origAlig = XX[i];
            
            origAlig -> Cleaning -> setTrimTerminalGapsFlag(terminalOnly);
            origAlig -> setKeepSequencesFlag(keepSeqs);

            set_window_size();

            if(blockSize != -1)
                origAlig -> setBlockSize(blockSize);

            if (create_or_use_similarity_matrix())
                return -2;

            print_statistics();

            clean_alignment();
            
            if((outfile != NULL) && (!appearErrors))
            {
                std::string outFileString = std::string(outfile);
                if (ReadWriteMachine.saveAlignment(outFileString, &oformats, origAlig) == false)
                {
                    debug.report(ErrorCode::ImpossibleToGenerate, new string[1] { "the output file"});
                    appearErrors = true;
                }

            }
            
        }
//         delete_variables();
        return 0;
    }
    
    origAlig -> Cleaning -> setTrimTerminalGapsFlag(terminalOnly);
    origAlig -> setKeepSequencesFlag(keepSeqs);

    set_window_size();

    if(blockSize != -1)
        origAlig -> setBlockSize(blockSize);

    if (create_or_use_similarity_matrix())
        return -2;

    print_statistics();

    clean_alignment();

    if(singleAlig == NULL)
    {
        singleAlig = origAlig;
        origAlig = NULL;
    }

    if(backtransFile != NULL)
    {
        singleAlig = backtranslationAlig -> getTranslationCDS(singleAlig);
    }

    if((svgOutFile != NULL) && (!appearErrors))
        if(!origAlig ->
                alignmentSummarySVG(*singleAlig, svgOutFile, compareVect))
        {
            debug.report(ErrorCode::ImpossibleToGenerate, new string[1] { "the SVG output file"});
            appearErrors = true;
        }
        
    if((htmlOutFile != NULL) && (!appearErrors))
        if(!origAlig ->
                alignmentSummaryHTML(*singleAlig, htmlOutFile, compareVect))
        {
            debug.report(ErrorCode::ImpossibleToGenerate, new string[1] { "the HTML output file"});
            appearErrors = true;
        }

    if((outfile != NULL) && (!appearErrors))
    {
        std::string outFileString = std::string(outfile);
        if (ReadWriteMachine.saveAlignment(outFileString, &oformats, singleAlig) == false)
        {
            debug.report(ErrorCode::ImpossibleToGenerate, new string[1] { "the output file"});
            appearErrors = true;
        }

    }

    else if((stats >= 0) && (!appearErrors))
        ReadWriteMachine.saveAlignment("", &oformats, singleAlig);

    if((columnNumbering) && (!appearErrors))
        singleAlig -> Statistics -> printCorrespondence();

//     delete_variables();

    return 0;
}

inline void trimAlManager::print_statistics()
{
    if (svgStatsOutFile != NULL)
    {
        std::string title = infile;
        std::string filename = svgStatsOutFile;
        std::string linename = "";
        std::string color = "";

        utils::streamSVG(NULL, NULL, 0, NULL, NULL, & title, & filename);

        if(origAlig-> Statistics -> calculateGapStats())
        {
            float acm = 0.0F;
            float x = 0;
            float y = 1.F;
            color = "red";
            linename = "gaps";
            utils::streamSVG(& x, & y, 0, & linename, & color, NULL, NULL);

            for(i = 0, acm = 0; i <= origAlig -> sgaps -> maxGaps; i++) {
                // If the columns' number with this gaps' number is not equal to zero, we will count the columns. 
                if(origAlig -> sgaps -> numColumnsWithGaps[i] != 0) {
                    // Compute and prints the accumulative values for the gaps in the alignment. 
                    acm += origAlig -> sgaps -> numColumnsWithGaps[i];
                    x = acm / origAlig -> sgaps -> residNumber;
                    y = 1.F - ((i * 1.0F)/origAlig -> sgaps ->sequenNumber);
                    utils::streamSVG(& x, & y, 0, & linename, & color, NULL, NULL);
                }
            }
        }

        if(origAlig -> Statistics -> calculateConservationStats())
        {
            color = "blue";
            linename = "conservation";
            float x = 0;
            float y = 1.F;
            utils::streamSVG(& x, & y, 0, & linename, & color, NULL, NULL);
            float refer, *vectAux;
            int i, num, acm;

            // Allocate memory 
            vectAux = new float[origAlig -> scons -> residues];

            // Select the conservation's value source and copy that vector in a auxiliar vector 
            if(origAlig -> scons -> MDK_Window != NULL)
                utils::copyVect(origAlig -> scons -> MDK_Window, vectAux, origAlig -> scons -> residues);
            else
                utils::copyVect(origAlig -> scons -> MDK, vectAux, origAlig -> scons -> residues);

            // Sort the auxiliar vector. 
            utils::quicksort(vectAux, 0, origAlig -> scons -> residues - 1);

            // Initializate some values 
            refer = vectAux[origAlig -> scons -> residues-1];
            acm = 0;
            num = 1;

            // Count the columns with the same conservation's value and compute this information to shows the accunulative
            // statistics in the alignment. 
            for(i = origAlig -> scons -> residues-2; i >= 0; i--) {
                acm++;

                if(refer != vectAux[i]) {
                    x = ((float) acm/origAlig -> scons -> residues ) ;
                    y = refer;
                    utils::streamSVG(& x, & y, 0, & linename, & color, NULL, NULL);
                    refer = vectAux[i];
                    num = 1;
                }
                else num++;
            }
            acm++;
            x = ((float) acm/origAlig -> scons -> residues ) ;
            y = refer;
            utils::streamSVG(& x, & y, 0, & linename, & color, NULL, NULL);

            // Deallocate the reserved memory. 
            delete [] vectAux;
        }

        utils::streamSVG(NULL, NULL, 0, NULL, NULL, NULL, NULL);
    }


//     if (sgt || sgc || scc || sct || sfc || sft)
    {
        if(sgc)
        {
            origAlig -> Statistics -> printStatisticsGapsColumns();
            stats++;
        }

        if(sgt)
        {
            origAlig -> Statistics -> printStatisticsGapsTotal();
            stats++;
        }

        if(scc)
        {
            origAlig -> Statistics -> printStatisticsConservationColumns();
            stats++;
        }

        if(sct)
        {
            origAlig -> Statistics -> printStatisticsConservationTotal();
            stats++;
        }

        if(sident)
        {
            origAlig -> printSeqIdentity();
            stats++;
        }

        if(soverlap) {
            origAlig -> printSeqOverlap();
            stats++;
        }

        if(compareset != -1)
        {
            if(sfc)
                compareFiles::printStatisticsFileColumns(*origAlig, compareVect);
            if(sft)
                compareFiles::printStatisticsFileAcl(*origAlig, compareVect);
        }
        cout << endl;
    }

}

inline bool trimAlManager::create_or_use_similarity_matrix()
{
    if((strict) || (strictplus) || (automated1) || (similarityThreshold != -1.0) || (scc == 1) || (sct == 1))
    {
        similMatrix = new similarityMatrix();

        // Simple Matrix
        if(matrixFile != NULL)
            similMatrix -> loadSimMatrix(matrixFile);

        // Alternative Matrix
        else if(alternative_matrix != -1)
        {
            similMatrix -> alternativeSimilarityMatrices(alternative_matrix, origAlig -> getAlignmentType());
        }

        // Default Matrices
        else {
            int alignDataType = origAlig -> getAlignmentType();
            if(alignDataType & SequenceTypes::AA)
                similMatrix -> defaultAASimMatrix();
            else if((alignDataType == SequenceTypes::DNA) || (alignDataType == SequenceTypes::RNA))
                similMatrix -> defaultNTSimMatrix();
            else if((alignDataType == (SequenceTypes::DNA | SequenceTypes::DEG)) || (alignDataType == (SequenceTypes::RNA | SequenceTypes::DEG)))
                similMatrix -> defaultNTDegeneratedSimMatrix();
        }

        if(!origAlig -> Statistics -> setSimilarityMatrix(similMatrix))
        {
            debug.report(ErrorCode::ImpossibleToProcessMatrix);
            return true;
        }
    }
    return false;
}

inline void trimAlManager::clean_alignment()
{

    if(nogaps)
        singleAlig = origAlig -> Cleaning -> cleanGaps(0, 0, getComplementary);

    else if(noallgaps)
        singleAlig = origAlig -> Cleaning -> cleanNoAllGaps(getComplementary);

    else if(gappyout)
        singleAlig = origAlig -> Cleaning -> clean2ndSlope(getComplementary);

    else if(strict)
        singleAlig = origAlig -> Cleaning -> cleanCombMethods(getComplementary, false);

    else if(strictplus)
        singleAlig = origAlig -> Cleaning -> cleanCombMethods(getComplementary, true);

    else if(automated1)
    {
        if(origAlig -> Cleaning -> selectMethod() == GAPPYOUT)
            singleAlig = origAlig -> Cleaning -> clean2ndSlope(getComplementary);
        else // STRICT
            singleAlig = origAlig -> Cleaning -> cleanCombMethods(getComplementary, false);
    }

    else if(consistencyThreshold != -1)
    {
        singleAlig = origAlig -> Cleaning -> cleanCompareFile(consistencyThreshold, conservationThreshold, compareVect, getComplementary);
    }

    else if((residuesOverlap != -1) && (sequenceOverlap != -1))
    {
        tempAlig = origAlig -> Cleaning -> cleanSpuriousSeq(residuesOverlap, (sequenceOverlap/100), getComplementary);

        singleAlig = tempAlig -> Cleaning -> cleanNoAllGaps(false);

        delete tempAlig;

    }

    else if(similarityThreshold != -1.0)
    {
        if(gapThreshold != -1.0)
            singleAlig = origAlig -> Cleaning -> clean(conservationThreshold, gapThreshold, similarityThreshold, getComplementary);
        else
            singleAlig = origAlig -> Cleaning -> cleanConservation(conservationThreshold, similarityThreshold, getComplementary);
    }

    else if(gapThreshold != -1.0)
        singleAlig = origAlig -> Cleaning -> cleanGaps(conservationThreshold, gapThreshold, getComplementary);

    else if((selectCols) || (selectSeqs))
    {

        if(delColumns != NULL)
        {
            num = delColumns[0];
            if(delColumns[num] >= origAlig -> getNumAminos())
            {
                debug.report(ErrorCode::SelectOnlyAccepts, new string[2] {"-selectcols", "residues" });
                appearErrors = true;
            }
            else
                singleAlig = origAlig -> Cleaning -> removeColumns(delColumns, 1, num, getComplementary);
        }

        if(delSequences != NULL)
        {
            num = delSequences[0];
            if(delSequences[num] >= origAlig -> getNumSpecies())
            {
                debug.report(ErrorCode::SelectOnlyAccepts, new string[2] {"-selectseqs", "sequences" });
                appearErrors = true;
            }
            else
            {
                tempAlig = origAlig -> Cleaning -> removeSequences(delSequences, 1, num, getComplementary);

                singleAlig = tempAlig -> Cleaning -> cleanNoAllGaps(false);

                delete tempAlig;
            }

        }
    }

    else if(maxIdentity != -1)
    {
        tempAlig = origAlig -> Cleaning -> getClustering (maxIdentity);

        singleAlig = tempAlig -> Cleaning -> cleanNoAllGaps(false);

        delete tempAlig;

    }
    else if(clusters != -1)
    {
        if(clusters > origAlig -> getNumSpecies())
        {
            debug.report(ErrorCode::MoreClustersThanSequences);
            appearErrors = true;
        }
        else
        {
            tempAlig = origAlig -> Cleaning -> getClustering(origAlig -> Cleaning -> getCutPointClusters(clusters));

            singleAlig = tempAlig -> Cleaning -> cleanNoAllGaps(false);

            delete tempAlig;

        }
    }
}

inline void trimAlManager::set_window_size()
{
    if(windowSize != -1)
    {
        gapWindow = windowSize;
        similarityWindow = windowSize;
    }
    else
    {
        if(gapWindow == -1)
            gapWindow = 0;
        if(similarityWindow == -1)
            similarityWindow = 0;
    }
    origAlig -> setWindowsSize(gapWindow, similarityWindow);
}

inline void trimAlManager::delete_variables()
{
    if (singleAlig != NULL)             delete singleAlig;
    if (origAlig != NULL)               delete origAlig;
    if (compareAlignmentsArray != NULL) delete [] compareAlignmentsArray;

    if (similMatrix != NULL)            delete similMatrix;
    if (delColumns != NULL)             delete [] delColumns;
    if (delSequences != NULL)           delete [] delSequences;

    if (filesToCompare != NULL)         delete [] filesToCompare;
    if (compareVect != NULL)            delete [] compareVect;

    if (outfile != NULL)                delete [] outfile;
    if (htmlOutFile != NULL)            delete [] htmlOutFile;

    if (infile != NULL)                 delete [] infile;
    if (matrixFile != NULL)             delete [] matrixFile;

    if(forceFile != NULL)               delete forceFile;
    if(backtransFile != NULL)           delete backtransFile;
    if(backtranslationAlig != NULL)     delete backtranslationAlig;
    
    if(vcfs != NULL)                    delete vcfs;
}

void trimAlManager::menu(void)
{
    cout << endl;
    cout << "trimAl v" << VERSION << ".rev" << REVISION  << " build[" << BUILD
         << "]. " << AUTHORS << endl << endl;

    cout << "trimAl webpage: http://trimal.cgenomics.org" << endl << endl;

    cout << "This program is free software: you can redistribute it and/or modify " << endl
         << "it under the terms of the GNU General Public License as published by " << endl
         << "the Free Software Foundation, the last available version." << endl << endl;

    cout << "Please cite:" << endl
         << "\t\ttrimAl: a tool for automated newAlignment  trimming in large-scale phylogenetic analyses."
         << "\n\t\tSalvador Capella-Gutierrez; Jose M. Silla-Martinez; Toni Gabaldon."
         << "\n\t\tBioinformatics 2009, 25:1972-1973." << endl << endl;

    cout << "Basic usage" << endl
         << "\ttrimal -in <inputfile> -out <outputfile> -(other options)." << endl << endl;

    cout << "Common options (for a complete list please see the User Guide or visit http://trimal.cgenomics.org):" << endl << endl;
    cout << "    -h                       " << "Print this information and show some examples." << endl;
    cout << "    --version                " << "Print the trimAl version." << endl << endl;

    cout << "    -in <inputfile>          " << "Input file in several formats (clustal, fasta, NBRF/PIR, nexus, phylip3.2, phylip)." << endl << endl;

    cout << "    -compareset <inputfile>  " << "Input list of paths for the files containing the alignments to compare." << endl;
    cout << "    -forceselect <inputfile> " << "Force selection of the given input file in the files comparison method." << endl << endl;

    cout << "    -backtrans <inputfile>   " << "Use a Coding Sequences file to get a backtranslation for a given AA alignment" << endl;
    cout << "    -ignorestopcodon         " << "Ignore stop codons in the input coding sequences" << endl;
    cout << "    -splitbystopcodon        " << "Split input coding sequences up to first stop codon appearance" << endl << endl;


    cout << "    -matrix <inpufile>       " << "Input file for user-defined similarity matrix (default is Blosum62)." << endl << endl;

    cout << "    -out <outputfile>        " << "Output newAlignment  in the same input format (default stdout). (default input format)" << endl;
    cout << "    -htmlout <outputfile>    " << "Get a summary of trimal's work in an HTML file." << endl;
    cout << "    -svgout <outputfile>     " << "Get a summary of trimal's work in an SVG file." << endl;
    cout << "    -sgvstats <outputfile>   " << "Get a graphic in SVG showing stats of the indicators used in the cleanse of the alignment." << endl << endl;

    cout << "    -keepheader              " << "Keep original sequence header including non-alphanumeric characters." << endl;
    cout << "                             " << "Only available for input FASTA format files. (future versions will extend this feature)" << endl << endl;

    cout << "    -nbrf                    " << "Output file in NBRF/PIR format" << endl;
    cout << "    -mega                    " << "Output file in MEGA format" << endl;
    cout << "    -nexus                   " << "Output file in NEXUS format" << endl;
    cout << "    -clustal                 " << "Output file in CLUSTAL format" << endl << endl;

    cout << "    -fasta                   " << "Output file in FASTA format" << endl;
    cout << "    -fasta_m10               " << "Output file in FASTA format. Sequences name length up to 10 characters." << endl << endl;

    cout << "    -phylip                  " << "Output file in PHYLIP/PHYLIP4 format" << endl;
    cout << "    -phylip_m10              " << "Output file in PHYLIP/PHYLIP4 format. Sequences name length up to 10 characters." << endl;
    cout << "    -phylip_paml             " << "Output file in PHYLIP format compatible with PAML" << endl;
    cout << "    -phylip_paml_m10         " << "Output file in PHYLIP format compatible with PAML. Sequences name length up to 10 characters." << endl;
    cout << "    -phylip3.2               " << "Output file in PHYLIP3.2 format" << endl;
    cout << "    -phylip3.2_m10           " << "Output file in PHYLIP3.2 format. Sequences name length up to 10 characters." << endl << endl;

    cout << "    -complementary           " << "Get the complementary alignment." << endl;
    cout << "    -colnumbering            " << "Get the relationship between the columns in the old and new alignment." << endl << endl;

    cout << "    -selectcols { n,l,m-k }  " << "Selection of columns to be removed from the alignment. Range: [0 - (Number of Columns - 1)]. (see User Guide)." << endl;
    cout << "    -selectseqs { n,l,m-k }  " << "Selection of sequences to be removed from the alignment. Range: [0 - (Number of Sequences - 1)]. (see User Guide)." << endl << endl;

    cout << "    -gt -gapthreshold <n>    " << "1 - (fraction of sequences with a gap allowed). Range: [0 - 1]" << endl;
    cout << "    -st -simthreshold <n>    " << "Minimum average similarity allowed. Range: [0 - 1]" << endl;
    cout << "    -ct -conthreshold <n>    " << "Minimum consistency value allowed.Range: [0 - 1]" << endl;
    cout << "    -cons <n>                " << "Minimum percentage of the positions in the original newAlignment  to conserve. Range: [0 - 100]" << endl << endl;

    cout << "    -nogaps                  " << "Remove all positions with gaps in the alignment." << endl;
    cout << "    -noallgaps               " << "Remove columns composed only by gaps." << endl;
    cout << "    -keepseqs                " << "Keep sequences even if they are composed only by gaps." << endl << endl;

    cout << "    -gappyout                " << "Use automated selection on \"gappyout\" mode. This method only uses "
         <<                                    "information based on gaps' distribution. (see User Guide)." << endl;
    cout << "    -strict                  " << "Use automated selection on \"strict\" mode. (see User Guide)." << endl;
    cout << "    -strictplus              " << "Use automated selection on \"strictplus\" mode. (see User Guide)."  << endl;
    cout << "                             " << "(Optimized for Neighbour Joining phylogenetic tree reconstruction)."<< endl << endl;

    cout << "    -automated1              " << "Use a heuristic selection of the automatic method based on similarity statistics. "
         <<                                    "(see User Guide). (Optimized for Maximum Likelihood phylogenetic tree reconstruction)." << endl << endl;

    cout << "    -terminalonly            " << "Only columns out of internal boundaries (first and last column without gaps) are " << endl;
    cout << "                             " << "candidated to be trimmed depending on the applied method" << endl;

    cout << "    -block <n>               " << "Minimum column block size to be kept in the trimmed alignment. Available with manual"
         <<                                    " and automatic (gappyout) methods" << endl << endl;


    cout << "    -resoverlap              " << "Minimum overlap of a positions with other positions in the column to be considered a "
         <<                                    "\"good position\". Range: [0 - 1]. (see User Guide)." << endl;
    cout << "    -seqoverlap              " << "Minimum percentage of \"good positions\" that a sequence must have in order to be conserved. Range: [0 - 100]"
         <<                                    "(see User Guide)." << endl << endl;

    cout << "    -clusters <n>            " << "Get the most Nth representatives sequences from a given alignment. Range: [1 - (Number of sequences)]" << endl;
    cout << "    -maxidentity <n>         " << "Get the representatives sequences for a given identity threshold. Range: [0 - 1]." << endl << endl;

    cout << "    -w <n>                   " << "(half) Window size, score of position i is the average of the window (i - n) to (i + n)." << endl;
    cout << "    -gw <n>                  " << "(half) Window size only applies to statistics/methods based on Gaps." << endl;
    cout << "    -sw <n>                  " << "(half) Window size only applies to statistics/methods based on Similarity." << endl;
    cout << "    -cw <n>                  " << "(half) Window size only applies to statistics/methods based on Consistency." << endl << endl;

    cout << "    -sgc                     " << "Print gap scores for each column in the input alignment." << endl;
    cout << "    -sgt                     " << "Print accumulated gap scores for the input alignment." << endl;
    cout << "    -ssc                     " << "Print similarity scores for each column in the input alignment." << endl;
    cout << "    -sst                     " << "Print accumulated similarity scores for the input alignment." << endl;
    cout << "    -sfc                     " << "Print sum-of-pairs scores for each column from the selected alignment" << endl;
    cout << "    -sft                     " << "Print accumulated sum-of-pairs scores for the selected alignment" << endl;
    cout << "    -sident                  " << "Print identity scores for all sequences in the input alignment. (see User Guide)." << endl;
    cout << "    -soverlap                " << "Print overlap scores matrix for all sequences in the input alignment. (see User Guide)." << endl << endl;
}

void trimAlManager::examples(void)
{

    cout << "Some Examples:" << endl << endl;

    cout << "1) Removes all positions in the newAlignment  with gaps in 10% or more of" << endl
         << "   the sequences, unless this leaves less than 60% of original alignment. " << endl
         << "   In such case, print the 60% best (with less gaps) positions." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -gt 0.9 -cons 60" << endl << endl;

    cout << "2) As above but, the gap score is averaged over a window starting" << endl
         << "   3 positions before and ending 3 positions after each column." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -gt 0.9 -cons 60 -w 3" << endl << endl;

    cout << "3) Use an automatic method to decide optimal thresholds, based in the gap scores" << endl
         << "   from input alignment. (see User Guide for details)." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -gappyout" << endl << endl;

    cout << "4) Use automatic methods to decide optimal thresholds, based on the combination " << endl
         << "   of gap and similarity scores. (see User Guide for details)." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -strictplus" << endl << endl;

    cout << "5) Use an heuristic to decide the optimal method for trimming the alignment. " << endl
         << "   (see User Guide for details)." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -automated1" << endl << endl;

    cout << "6) Use residues and sequences overlap thresholds to delete some sequences from the " << endl
         << "   alignemnt. (see User Guide for details)." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -resoverlap 0.8 -seqoverlap 75" << endl << endl;

    cout << "7) Selection of columns to be deleted from the alignment. The selection can " << endl
         << "   be a column number or a column number interval. Start from 0" << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -selectcols { 0,2,3,10,45-60,68,70-78 }" << endl << endl;

    cout << "8) Get the complementary newAlignment  from the newAlignment  previously trimmed." << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -selectcols { 0,2,3,10,45-60,68,70-78 } -complementary" << endl << endl;

    cout << "9) Selection of sequences to be deleted from the alignment. Start in 0" << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -selectseqs { 2,4,8-12 } " << endl << endl;

    cout << "10) Select the 5 most representative sequences from the alignment" << endl << endl;

    cout << "   trimal -in <inputfile> -out <outputfile> -clusters 5 " << endl << endl;
}



