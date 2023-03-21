#include <iostream>
#include <fstream>
#include <cstdint>

/***************
 * Structure that holds the argments
 * that setup the cache only a struct
 * to make it easy to pass around
 ***************/
struct cacheArguments
{
    unsigned int numberofSetsExp;
    unsigned int numberOfBlocksExp;
    unsigned int numberOfWordsPerBlockExp;
};

#define EXPONENT_CONVERSION(x) (1ul << (x))

typedef struct Block Block;

struct Block
{
    size_t tag;
    bool dirty;
    uint64_t lru;
};

typedef struct Set Set;

struct Set
{
    Block *block;

    void incrementAllLRU(size_t numberOfBlocks)
    {
        for (size_t i = 0; i < numberOfBlocks; i++)
        {
            block[i].lru += 1;
        }
    }

    bool lookupTag(size_t tag, size_t numberOfBlocks)
    {
        bool hit = false;
        uint64_t maxLRU = 0;
        size_t lruIndex;
        for (size_t i = 0; i < numberOfBlocks; i++)
        {
            if (block[i].tag == tag && block[i].dirty)
            {
                hit = true;
                block[i].lru = 0;
            }
            else if (block[i].lru > maxLRU)
            {
                maxLRU = block[i].lru;
                lruIndex = i;
            }
        }
        if (!hit)
        {
            block[lruIndex].tag = tag;
            block[lruIndex].lru = 0;
            block[lruIndex].dirty = true;
        }
        return hit;
    }
};

/**********************************
 * parseArgs specific to this program will pasrse the arguments
 * and use them to set up the program state will return false
 * if any of the arguments would cause errors in the running
 * of the program
 **********************************/
bool parseArgs(int argc, char *argv[], cacheArguments &cacheArgs, std::ifstream &file)
{

    bool parseSuccessful = true;

    // open file
    file.open(argv[1], std::fstream::binary | std::fstream::in);

    // check for obvious failures
    if (argc != 5 || !file)
    {
        std::cout << "The incorrect arguments" << std::endl;
        parseSuccessful = false;
    }
    else
    {
        sscanf(argv[2], "%d", &cacheArgs.numberofSetsExp);
        sscanf(argv[3], "%d", &cacheArgs.numberOfBlocksExp);
        sscanf(argv[4], "%d", &cacheArgs.numberOfWordsPerBlockExp);
    }

    return parseSuccessful;
}

void cleanupCache(Set *cache, const cacheArguments &cacheArgs)
{
    for (unsigned int i = 0; i < EXPONENT_CONVERSION(cacheArgs.numberofSetsExp); i++)
    {
        delete[] cache[i].block;
    }
    delete[] cache;
}

int main(int argc, char *argv[])
{
    // create file that will be used to test the caching algorithm
    std::ifstream programFile;

    // get and parse arguments
    cacheArguments cacheArgs;

    if (!parseArgs(argc, argv, cacheArgs, programFile))
    {
        std::cout << "There were issues parsing the arguments provided suspending program early with a return code of -1" << std::endl;
        std::cout << "Expected arguments are <valid file> <x the number of direct mapped sets>\n\t <y the number of blocks per set> <z the number of cached words per block>" << std::endl;
        return -1;
    }

    /************************Setup**********************************/
    size_t mask = (~((~0ul) << cacheArgs.numberofSetsExp)) << cacheArgs.numberOfWordsPerBlockExp;
    Set *cache = new Set[EXPONENT_CONVERSION(cacheArgs.numberofSetsExp)];
    uint64_t accesses = 0;
    uint64_t hits = 0;
    std::string line;
    size_t addr;
    size_t set;
    size_t tag;

    // initialize individual sets
    for (unsigned int i = 0; i < EXPONENT_CONVERSION(cacheArgs.numberofSetsExp); i++)
    {
        cache[i].block = new Block[EXPONENT_CONVERSION(cacheArgs.numberOfBlocksExp)];
        for (unsigned int j = 0; j < EXPONENT_CONVERSION(cacheArgs.numberOfBlocksExp); j++)
        {
            cache[i].block[j] = {0, false, 0};
        }
    }

    /************************Loop for file processing**************************************/
    while (std::getline(programFile, line))
    {
        try
        {
            addr = std::stoull(line, nullptr, 16); // essentailly read in the hex characters as hex
        }
        catch (std::out_of_range &e)
        {
            std::cout << "There was an issue reading line " << accesses + 1 << " stoull threw and out of range exception." << std::endl;
            // cleanup
            cleanupCache(cache, cacheArgs);
            programFile.close();
            return -1;
        }
        set = (addr & mask) >> cacheArgs.numberOfWordsPerBlockExp;
        tag = (size_t)(addr >> (cacheArgs.numberofSetsExp + cacheArgs.numberOfWordsPerBlockExp));

        // increment all lrus in the set
        cache[set].incrementAllLRU(EXPONENT_CONVERSION(cacheArgs.numberOfBlocksExp));

        accesses++;

        if (cache[set].lookupTag(tag, EXPONENT_CONVERSION(cacheArgs.numberOfBlocksExp)))
        {
            hits++;
        }
    }

    std::cout << "hits: " << hits << "/" << accesses << std::endl;

    // cleanup
    cleanupCache(cache, cacheArgs);
    programFile.close();
    return 0;
}