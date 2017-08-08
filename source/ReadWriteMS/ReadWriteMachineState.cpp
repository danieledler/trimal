#include "../../include/ReadWriteMS/ReadWriteMachineState.h"
#include "../../include/ReadWriteMS/ReadWriteBaseState.h"
#include "../../include/newAlignment.h"

#include "../../include/ReadWriteMS/clustal_state.h"
#include "../../include/ReadWriteMS/fasta_state.h"
#include "../../include/ReadWriteMS/pir_state.h"
#include "../../include/ReadWriteMS/phylip32_state.h"
#include "../../include/ReadWriteMS/phylip40_state.h"
#include "../../include/ReadWriteMS/phylip_paml_state.h"
#include "../../include/ReadWriteMS/nexus_state.h"
#include "../../include/ReadWriteMS/mega_interleaved_state.h"
#include "../../include/ReadWriteMS/mega_sequential_state.h"
#include "../../include/ReadWriteMS/htmlreport_state.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>
#include <sstream>

ReadWriteMS::ReadWriteMS()
{
    addState(new ClustalState(this));
    addState(new FastaState(this));
    addState(new PirState(this));
    addState(new Phylip32State(this));
    addState(new Phylip40State(this));
    addState(new PhylipPamlState(this));
    addState(new NexusState(this));
    addState(new MegaInterleavedState(this));
    addState(new MegaSequentialState(this));
    addState(new HTMLState(this));
};

ReadWriteMS::~ReadWriteMS()
{
    for(ReadWriteBaseState* child : available_states) {
        delete child;
    }
}

void ReadWriteMS::addState(ReadWriteBaseState* newState)
{
    this -> available_states.push_back(newState);
}

newAlignment* ReadWriteMS::loadAlignment(std::string inFile)
{
    // Check input file.
    ifstream inFileHandler;
    inFileHandler.open(inFile);
    if (!inFileHandler.is_open())
    {
        cerr << "ERROR: Couldn't open input file " << inFile << endl << endl;
        return nullptr;
    }
    else if (inFileHandler.peek() == std::ifstream::traits_type::eof())
    {
        cerr << "ERROR: " << inFile << " is empty" << endl << endl;
        return nullptr;
    }

    ReadWriteBaseState* inState = NULL;
    int format_value = 0;
    int temp_value = 0;

    for(ReadWriteBaseState* state : available_states)
    {
        temp_value = state -> CheckAlignment(&inFileHandler);

        if (temp_value > format_value)
        {
            temp_value = format_value;
            inState = state;
        }
    }

    if (inState == NULL)
    {
        cerr << "ERROR: Cannot recognize input format of file " << inFile << endl << endl;
        inFileHandler.close();
        return nullptr;
    }

    newAlignment* alignment = inState->LoadAlignment(inFile);
    inFileHandler.close();
    return alignment;
}

bool ReadWriteMS::saveAlignment(std::string outPattern, std::vector< std::string >* outFormats, newAlignment* alignment)
{
    
    string filename;
    int start, end;
    if (alignment->filename == "")
    {
        filename = utils::ReplaceString(outPattern, "[in]", "NoInputFileName");
    }
    else
    {
        start = max((int)outPattern.find_last_of("/"), 0);
        end = outPattern.find_last_of(".");
        filename = utils::ReplaceString(outPattern, "[in]", alignment->filename.substr(start, end-start));
    }

    
    if (outPattern == "")
    {
        if (outFormats->size() == 0) 
        {
            outFormats->push_back("fasta");
        }
        if (outFormats->size() == 1)
        {
            for(ReadWriteBaseState* state : available_states)
            {
                if (state->RecognizeOutputFormat( outFormats->at(0) ))
                {
                    return state->SaveAlignment(alignment, &cout, &filename);
                }
            }
            cerr << "ERROR: Format " << outFormats->at(0) << " wasn't recognized" << endl << endl;
            return false;
        }
        else
        {
            cerr << "ERROR: You must specify only one output format if you don't provide an output file pattern" << endl << endl;
            return false;
        }
    }
    else
    {
        if (outFormats->size() == 0) 
        {
            outFormats->push_back("fasta");
        }
        if (outFormats->size() == 1)
        {
            for(ReadWriteBaseState* state : available_states)
            {
                if (state->RecognizeOutputFormat( outFormats->at(0) ))
                {
                    utils::ReplaceStringInPlace(filename, "[extension]", state->extension);
                    utils::ReplaceStringInPlace(filename, "[format]", state->name);
                    
                    ofstream outFileHandler;
                    outFileHandler.open(filename);
                    
                    return state->SaveAlignment(alignment, &outFileHandler, &alignment->filename);
                }
            }
            cerr << "ERROR: Format specified wasn't recognized" << endl << endl;
            return false;
        }
        
        // Tranform the list of std::string states to a ReadWriteBaseState list.
        bool recognized = true;
        std::vector<ReadWriteBaseState*> outStates = std::vector<ReadWriteBaseState*>();
        {
            recognized = false;
            for (std::string outFormat : *outFormats)
            {
                for(ReadWriteBaseState* state : available_states)
                {
                    if (state->RecognizeOutputFormat(outFormat))
                    {
                        outStates.push_back(state);
                        recognized = true;
                    }
                }
                if (!recognized)
                {
                    cerr << "ERROR: Cannot recognize output format " << outFormat << endl << endl;
                    return false; 
                }
            }
        }
        

        ofstream outFileHandler;
        bool isCorrect = true;
        string filename_2;
        for (ReadWriteBaseState * state : outStates)
        {
            filename_2 = utils::ReplaceString(filename, "[extension]", state->extension);
            utils::ReplaceStringInPlace(filename_2, "[format]", state->name);
            outFileHandler.open(filename_2);
            if(!state -> SaveAlignment(alignment, &outFileHandler, &alignment->filename))
            {
                cerr << "ERROR: Alignment couldn't be saved on " << state->name << " format" << endl << endl;
                isCorrect = false;
            }
            
            outFileHandler.close();
        }
        return isCorrect;
    
    }
    return false;
}

void ReadWriteMS::processFile(
    std::vector< std::string >* inFiles,
    std::string* outPattern,
    std::vector< std::string >* outFormats)
{

    // Get all the output format states needed.
    std::vector<ReadWriteBaseState*> outStates = std::vector<ReadWriteBaseState*>();
    {
        bool recognized;
        for (std::string outFormat : *outFormats)
        {
            recognized = false;
            for(ReadWriteBaseState* state : available_states)
            {
                if (state->RecognizeOutputFormat(outFormat))
                {
                    outStates.push_back(state);
                    recognized = true;
                    break;
                }
            }

            if (!recognized)
            {
                cerr << "Cannot recognize output format " << outFormat << endl;
                return;
            }
        }
    }

    // Process input files one by one.
    ReadWriteBaseState* inState = NULL;
    ifstream inFileHandler;
    int format_value = 0;
    int temp_value = 0;

    for (std::string inFile : *inFiles)
    {
        // Open file.
        inFileHandler.open(inFile);
        if (!inFileHandler.is_open())
        {
            cerr << "ERROR: Couldn't open input file " << inFile << endl;
            return;
        }
        else if (inFileHandler.peek() == std::ifstream::traits_type::eof())
        {
            cerr << "ERROR: " << inFile << " is empty" << endl;
            return;
        }

        inState = NULL;
        format_value = 0;
        temp_value = 0;

        // Get format State that can handle this alignment.
        for(ReadWriteBaseState* state : available_states)
        {
            temp_value = state -> CheckAlignment(&inFileHandler);

            if (temp_value > format_value)
            {
                temp_value = format_value;
                inState = state;
            }
        }

        // Check if there is a format State to handle the alignment.
        if (inState == NULL)
        {
            cerr << "Cannot recognize input format of file " << inFile << endl;
            inFileHandler.close();
            return;
        }

        // Load alignment one by one and store it on each of the formats specified.
        newAlignment* alignment = inState->LoadAlignment(inFile);
        int start, end;
        inFileHandler.close();
        {
            string filename;
            ofstream outFileHandler;
            for (ReadWriteBaseState * state : outStates)
            {
                start = max((int)inFile.find_last_of("/"), 0);
                end = inFile.find_last_of(".");
                filename = utils::ReplaceString(*outPattern, "[in]", inFile.substr(start, end-start));
                utils::ReplaceStringInPlace(filename, "[extension]", state->extension);
                utils::ReplaceStringInPlace(filename, "[format]", state->name);

                outFileHandler.open(filename);
                if (this->hasOutputFile)
                    state -> SaveAlignment(alignment, &outFileHandler, &inFile);
                else
                    state -> SaveAlignment(alignment, &cout, &inFile);
                outFileHandler.close();
            }
        }
        if (alignment != nullptr)
            delete alignment;
    }
}

std::string ReadWriteMS::getFileFormatName(std::string inFile)
{
    ifstream inFileHandler;

    // Open file.
    inFileHandler.open(inFile);
    if (!inFileHandler.is_open())
    {
        cerr << "Couldn't open input file " << inFile << endl;
        return "Unknown";
    }
    else if (inFileHandler.peek() == std::ifstream::traits_type::eof())
    {
        cerr << "ERROR: " << inFile << " is empty" << endl;
        return "None";
    }

    ReadWriteBaseState* inState = NULL;
    int format_value = 0;
    int temp_value = 0;

    // Get format State that can handle this alignment.
    for(ReadWriteBaseState* state : available_states)
    {
        temp_value = state -> CheckAlignment(&inFileHandler);

        if (temp_value > format_value)
        {
            temp_value = format_value;
            inState = state;
        }
    }

    // Check if there is a format State to handle the alignment.
    if (inState == NULL)
    {
        inFileHandler.close();
        return "Unknown";
    }

    return inState->name;
}

std::string ReadWriteMS::getInputFormatsAvailable()
{
    std::stringstream ss("");

    for (ReadWriteBaseState* state : available_states)
    {
        if (state->canLoad)
            ss << state->name << ", " ;
    }
    ss.seekp(-2, std::ios_base::end);
    ss << "  ";

    return ss.str();

}

std::string ReadWriteMS::getOutputFormatsAvailable()
{
    std::stringstream ss("");

    for (ReadWriteBaseState* state : available_states)
    {
        if (state->canSave)
            ss << state->name << ", " ;
    }
    ss.seekp(-2, std::ios_base::end);
    ss << "  ";

    return ss.str();

}
