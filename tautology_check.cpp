#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <cmath>
#include <cstring>

//debugging flag
static bool debug = false;
//cube valid information index offsets
static int cubeUniOff = 0;

enum varVals {
    COMP = 0,
    TRUE, 
    DC
};

enum varPolarity {
    SINGLE = 1,
    DOUBLE
};

enum cofactors {
    POS = 1,
    NEG
};

typedef struct {
    int polarity;
    int numTrue;
    int numComp;
    int numDC;
    int dependence;
    int balance;
} BoolVar;

typedef struct {
    int numVars;
    int numCubes;
    unsigned char **cubes;
    BoolVar **vars;
    bool unate;
    bool containsUniCube;
} CubeList;

void printCurrentCubeList(CubeList *myCubeList)
{ 
    for (int icube = 0; icube < myCubeList->numCubes; icube++) {
        printf("cube[%i]: ", icube);

        for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
            if (myCubeList->cubes[icube][ivar] == DC) {
                printf("-");
            } else {
                printf("%i", myCubeList->cubes[icube][ivar]);
            }
        }
        printf("\n");
    }
}

void freeCubeList(CubeList* myCubeList)
{
    for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
        free(myCubeList->vars[ivar]);
    }
    free(myCubeList->vars);

    for (int icube = 0; icube < myCubeList->numCubes; icube++) {
        free(myCubeList->cubes[icube]);
    }
    free(myCubeList->cubes);

    free(myCubeList);
}

void updateVarInfo(CubeList *myCubeList) 
{
//    if(debug) printf("updating variable info for current cube list\n");

    //clear previous var info
    for (int i = 0; i < myCubeList->numVars; i++) {
        myCubeList->vars[i]->numTrue = 0;
        myCubeList->vars[i]->numComp = 0;
        myCubeList->vars[i]->numDC = 0;
        myCubeList->vars[i]->dependence = 0;
        myCubeList->vars[i]->balance = 0;
    }

    //parse var colums of cube matrix to populate boolvar info
    for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
        for (int icube = 0; icube < myCubeList->numCubes; icube++) {
//            if(debug) printf("boolvar[%i] in cube[%i] = %i\n", ivar, icube, myCubeList->cubes[icube][ivar]);

            if (myCubeList->cubes[icube][ivar] == TRUE) { 
                myCubeList->vars[ivar]->numTrue++;
                myCubeList->vars[ivar]->dependence++;
            } else if (myCubeList->cubes[icube][ivar] == COMP) {
                myCubeList->vars[ivar]->numComp++;
                myCubeList->vars[ivar]->dependence++;
            } else {
                myCubeList->vars[ivar]->numDC++;
            }
        }

        //determine whether or not var is single polarity 
        if ( (myCubeList->vars[ivar]->numTrue > 0 && myCubeList->vars[ivar]->numComp == 0) ||
             (myCubeList->vars[ivar]->numComp > 0 && myCubeList->vars[ivar]->numTrue == 0) ||
             (myCubeList->vars[ivar]->numDC == myCubeList->numCubes) )

        { 
            myCubeList->vars[ivar]->polarity = SINGLE;
            myCubeList->vars[ivar]->balance = -1;
        } else {
            myCubeList->vars[ivar]->polarity = DOUBLE;
            myCubeList->vars[ivar]->balance = abs(myCubeList->vars[ivar]->numTrue - myCubeList->vars[ivar]->numComp);
        }
    }
}

void checkForUnateCubeList(CubeList *myCubeList)
{
    for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
        if (myCubeList->vars[ivar]->polarity == DOUBLE) {
            myCubeList->unate = false;
            return;
        }
    }

    myCubeList->unate = true;

    if(debug) printf("\tcube list is unate\n");
}

CubeList *updateCofactorCubeList(CubeList *myCubeList, int varInd, int cofactor)
{
    bool updateDC = false;
    CubeList *subCubeList = NULL;
    int subListCubeCount = 0;

    if(debug)  {
        if (cofactor == POS) { printf("[SPLITTING] updating positive cofactor cube list for var[%i]\n", varInd); }
        if (cofactor == NEG) { printf("[SPLITTING] updating negative cofactor cube list for var[%i]\n", varInd); }
    }

    //create subset of cubelist for modification
    if ((subCubeList = (CubeList*)malloc(sizeof(CubeList))) == NULL) {
        printf("MALLOC ERROR!\n");
        exit(1);
    }
    subCubeList->numVars = myCubeList->numVars;
    subCubeList->containsUniCube = false;

    if ((subCubeList->vars = (BoolVar**)malloc(myCubeList->numVars*sizeof(BoolVar*))) == NULL) {
        printf("CALLOC ERROR!\n");
        exit(1);
    }
    //copy vars array without the current cofactor variable
    for (int i = 0; i < myCubeList->numVars; i++) {
        if ((subCubeList->vars[i] = (BoolVar*)malloc(sizeof(BoolVar))) == NULL) {
            printf("CALLOC ERROR!\n");
            exit(1);
        }

        memcpy(subCubeList->vars[i], myCubeList->vars[i], sizeof(BoolVar));
    }

    if ((subCubeList->cubes = (unsigned char**)malloc(myCubeList->numCubes*sizeof(unsigned char*))) == NULL) {
        printf("CALLOC ERROR!\n");
        exit(1);
    }

    for (int icube = 0; icube < myCubeList->numCubes; icube++) {
        updateDC = false;

        if ( (cofactor == POS && myCubeList->cubes[icube][varInd] == COMP) ||
             (cofactor == NEG && myCubeList->cubes[icube][varInd] == TRUE) )
        { 
            //do not copy cube into sublist and invalidate cube in global cube list

            if(debug) printf("\tremoving cube[%i] from sublist\n", icube);    

            continue;
        } 

        if ((subCubeList->cubes[subListCubeCount] = (unsigned char*)calloc(myCubeList->numVars + 1, sizeof(unsigned char))) == NULL) {
            printf("CALLOC ERROR!\n");
            exit(1);
        }
        memcpy(subCubeList->cubes[subListCubeCount], myCubeList->cubes[icube], 
               (myCubeList->numVars + 1) * sizeof(unsigned char));

        if ( (cofactor == POS && myCubeList->cubes[icube][varInd] == TRUE) ||
             (cofactor == NEG && myCubeList->cubes[icube][varInd] == COMP) )
        { 
            //change variable in cube to don't care
            subCubeList->cubes[subListCubeCount][varInd] = DC;
            subCubeList->cubes[subListCubeCount][cubeUniOff]++;
            
            updateDC = true;

            if(debug) printf("\tchanging var[%i] in cube[%i] to don't care\n", varInd, icube);
        }


        //see if universal cube is now present
        if (updateDC) {
            if (subCubeList->cubes[subListCubeCount][cubeUniOff] == subCubeList->numVars) {
                subCubeList->containsUniCube = true;
            }
        }

        subListCubeCount++;
    }
    
    subCubeList->numCubes = subListCubeCount;

    updateVarInfo(subCubeList);

    checkForUnateCubeList(subCubeList);

    return subCubeList;
}

int getMostBinateVar(CubeList *myCubeList)
{
    int ind = 0;
    int maxDep = 0;
    bool tie = false;
    std::vector<int> tieVars;

    for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
        if (myCubeList->vars[ivar]->dependence > maxDep) {
            maxDep = myCubeList->vars[ivar]->dependence;
            ind = ivar;
        } else if (myCubeList->vars[ivar]->dependence == maxDep && maxDep > 0) {
            tie = true;
            tieVars.push_back(ivar);
        }
    }

    if (tie) {
        int minBal = 100;

        for (int ivar = 0; ivar < int(tieVars.size()); ivar++) {
            if (myCubeList->vars[ivar]->balance < minBal) {
                minBal = myCubeList->vars[ivar]->balance;
                ind = tieVars[ivar]; 
            }
        }
    }

    return ind;
}

bool checkForDualPolaritySingleVarCubes(CubeList *myCubeList) 
{
    //key is var index, value is array of cubes with single variable key
    std::map<int, std::vector<int> > singleVarCubes;

    if(debug) printf("[RULE 3] checking for single var cubes with dual polarity\n");

    for (int icube = 0; icube < myCubeList->numCubes; icube++) {
        if (myCubeList->cubes[icube][cubeUniOff] == myCubeList->numVars - 1) {
            //determine which variable is single
            for(int ivar = 0; ivar < myCubeList->numVars; ivar++) {
                if (myCubeList->cubes[icube][ivar] != DC) {
                    singleVarCubes[ivar].push_back(icube);
                    if(debug) printf("\tsingle variable[%i] in cube[%i]\n", ivar, icube);
                    break;
                }
            }
        } 
    }

    for (std::map<int, std::vector<int> >::iterator it = singleVarCubes.begin(); it != singleVarCubes.end(); it++) {
        bool comp = false, norm = false;
        for (int icube = 0; icube < int(it->second.size()); icube++) {
            if (myCubeList->cubes[it->second[icube]][it->first] == COMP) {
                if(debug) printf("\tvariable[%i] exists in complement form\n", it->first);
                comp = true;
            } else if (myCubeList->cubes[it->second[icube]][it->first] == TRUE) {
                if(debug) printf("\tvariable[%i] exists in norm form\n", it->first);
                norm = true;
            }
        }

        if (comp && norm) {
            if(debug) printf("\tsingle variable %i has dual polarity\n", it->first);
            return true;
        }
    }

    return false;
}

bool checkForTautology(CubeList *myCubeList) 
{ 
    bool taut = false;

    if (myCubeList->unate) {
        if(debug) printf("Current cubelist is unate\n");
        //if universal cube exists in list, cube list is a tautology
        //else if universal cube is missing, cube list is not a tautology
        if (myCubeList->containsUniCube) {
            if(debug) {
                printf("[TAUTOLOGY] Current cubelist contains universal cube!\n");
            }
                
            taut = true;
        } else {
            if(debug) {
                printf("[NOT TAUTOLOGY] Current cubelist is missing universal cube!\n");
            }
                
            taut = false;
        }
        
        freeCubeList(myCubeList);
    } else if (checkForDualPolaritySingleVarCubes(myCubeList)) {
        if(debug) printf("[TAUTOLOGY] Current cubelist contains single variable cubes with dual polarity!\n");

        freeCubeList(myCubeList);

        taut = true;
    } else {
        int binateVar = 0;
        CubeList *posCofacCubeList = NULL;
        CubeList *negCofacCubeList = NULL;
        bool posCofacTautology = false;
        bool negCofacTautology = false;

        binateVar = getMostBinateVar(myCubeList);
        if(debug) printf("[UNKNOWN] splitting on variable[%i]\n", binateVar); 

        posCofacCubeList = updateCofactorCubeList(myCubeList, binateVar, POS);
        negCofacCubeList = updateCofactorCubeList(myCubeList, binateVar, NEG);
        
        if(debug) { 
            printf("Positive cofactor Cube List...\n");
            printCurrentCubeList(posCofacCubeList); 
        }
        posCofacTautology = checkForTautology(posCofacCubeList);

        if(debug) { 
            printf("Negative cofactor cube list...\n");
            printCurrentCubeList(negCofacCubeList); 
        }
        negCofacTautology = checkForTautology(negCofacCubeList);

        taut = posCofacTautology && negCofacTautology;
    }
    
    return taut;
}

int main(int argc, char* argv[])
{
    time_t t0 = time(NULL);
    int cubeCount = 0;
    int numVars, numCubes = 0;
    char *cubeStr = NULL;
    CubeList *myCubeList = NULL;

    if (argc < 1) {
        std::cout << "Usage: " << argv[0] << " [-d] < input_cover_file > output_cover_file " << std::endl;
        exit(0);
    } else if (argc == 2 && std::string(argv[1]) == "-d") {
        debug = true;
    }

    std::cin >> numVars;
    std::cin >> numCubes;

    //update cube info array index offsets
    cubeUniOff = numVars;

    if ((myCubeList = (CubeList*)malloc(sizeof(CubeList))) == NULL) {
        printf("MALLOC ERROR!\n");
        exit(1);
    }
    myCubeList->numVars = numVars;
    myCubeList->numCubes = numCubes;
    myCubeList->containsUniCube = false;

    if ((myCubeList->vars = (BoolVar**)malloc(numVars*sizeof(BoolVar*))) == NULL) {
        printf("CALLOC ERROR!\n");
        exit(1);
    }
    for (int i = 0; i < numVars; i++) {
        if ((myCubeList->vars[i] = (BoolVar*)malloc(sizeof(BoolVar))) == NULL) {
            printf("CALLOC ERROR!\n");
            exit(1);
        }

        myCubeList->vars[i]->numTrue = 0;
        myCubeList->vars[i]->numComp = 0;
        myCubeList->vars[i]->numDC = 0;
        myCubeList->vars[i]->dependence = 0;
        myCubeList->vars[i]->balance = 0;
    }


    if ((cubeStr = (char *)malloc((numVars+1)*sizeof(char))) == NULL) {
        printf("CALLOC ERROR!\n");
        exit(1);
    }

    if ((myCubeList->cubes = (unsigned char**)malloc(numCubes*sizeof(unsigned char*))) == NULL) {
        printf("CALLOC ERROR!\n");
        exit(1);
    }
    for (int i = 0; i < numCubes; i++) {
        if ((myCubeList->cubes[i] = (unsigned char*)calloc(numVars + 1, sizeof(unsigned char))) == NULL) {
            printf("CALLOC ERROR!\n");
            exit(1);
        }
    }

    //parse file and populate cube arrays 
    while (cubeCount < numCubes) {
        std::cin >> cubeStr;

        if(debug) printf("input cube string: %s\n", cubeStr);

        //parse cubes and populate cube arrays
        for (int ivar = 0; ivar < numVars; ivar++) {
            if (cubeStr[ivar] == '0') {
                myCubeList->cubes[cubeCount][ivar] = COMP;
            } else if (cubeStr[ivar] == '1') {
                myCubeList->cubes[cubeCount][ivar] = TRUE;
            } else if (cubeStr[ivar] == '-' || cubeStr[ivar] == '_') {
                myCubeList->cubes[cubeCount][ivar] = DC;
                myCubeList->cubes[cubeCount][cubeUniOff]++;
            } else {
                printf("INVALID CHARACTER IN CUBE\n");
                exit(1);
            }

        }

        if (myCubeList->cubes[cubeCount][cubeUniOff] == numVars) {
            printf("COVER IS A TAUTOLOGY\n");
            //TODO: write tautology cover file
            return 0;
        }

        cubeCount++;
    }

    free(cubeStr);

    updateVarInfo(myCubeList);

    if (debug) {
        for (int icubes = 0; icubes < numCubes; icubes++) {
            printf("cube[%i]: ", icubes);
            for (int ivars = 0; ivars < numVars; ivars++) {
                if (myCubeList->cubes[icubes][ivars] == DC) {
                    printf("-");
                } else {
                    printf("%i", myCubeList->cubes[icubes][ivars]);
                }
            }
            printf("\n");
        }

        for (int ivar = 0; ivar < numVars; ivar++) {
            printf("boolvar[%i]\n", ivar);
            if (myCubeList->vars[ivar]->polarity == SINGLE) {
                printf("\tpolarity = single\n");
            } else {
                printf("\tpolarity = double\n");
            }
            printf("\tnumTrue = %i\n", myCubeList->vars[ivar]->numTrue);
            printf("\tnumComp = %i\n", myCubeList->vars[ivar]->numComp);
            printf("\tnumDC = %i\n", myCubeList->vars[ivar]->numDC);
            printf("\tdependence = %i\n", myCubeList->vars[ivar]->dependence);
            printf("\tbalance = %i\n", myCubeList->vars[ivar]->balance);
        }
    }

    checkForUnateCubeList(myCubeList);

    if(debug) {
        printf("Initial cube list...\n");
        printCurrentCubeList(myCubeList);
    }

    if (checkForTautology(myCubeList)) {
        printf("COVER IS A TAUTOLOGY\n");
    } else { 
        printf("COVER IS NOT A TAUTOLOGY\n");
    }


    time_t tf = time(NULL);
    printf("Run time: %i hour(s), %i min(s), %i (secs)\n",
            int((tf - t0)/3600), int(((tf - t0)%3600)/60), int((tf - t0)%60));

    return 0;
}
