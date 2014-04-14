#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <cstring>

//debugging flag
static bool debug = false;
//cube array information index offsets
static int cubeValifOff = 0;
static int cubeUniOff = 0;

enum varPolarity {
    SINGLE = 1,
    DOUBLE
};

typedef struct {
    int polarity;
    int numTrue;
    int numComp;
    int numDC;
    int dependence;
} BoolVar;

typedef struct {
    int numVars;
    int numCubes;
    unsigned char **cubes;
    BoolVar **vars;
    bool unate;
} CubeList;

void checkForUnateCubeList(CubeList *myCubeList)
{
    for (int ivar = 0; ivar < myCubeList->numVars; ivar++) {
        if (myCubeList->vars[ivar]->polarity == DOUBLE) {
            myCubeList->unate = false;
            return;
        }
    }

    myCubeList->unate = true;

    if(debug) printf("cube list is unate\n");
}

bool hasUniversalCube(myCubeList)
{
    for (int icube = 0; icube < myCubeList->numCubes; icube++) { 
        if (myCubeList->cubes[icube][cubeValidOff]) {
            if (myCubeList->cubes[icube][cubeUnivOff] == numVars*2) {
                printf("List containts universal cube\n");
                return true;
            }
        }
    }

    if(debug) printf("Universal cube missing\n");

    return false;
}

bool checkForTautology(CubeList *myCubeList) 
{ 
    bool tautology = false;

    if (myCubeList->unate) {
        //if universal cube exists in list, cube list is a tautology
        //else if universal cube is missing, cube list is not a tautology
        if (hasUniversalCube(myCubeList)) {
            tautology = true;
        }
//TODO: any other termination rules work*/
/*    } else if () {
        //TODO: set appropriate tautology value
*/    } else {
        bool posCofacTautology = false;
        bool negCofacTautology = false;

        //TODO: get most binate variable in cubelist
        //TODO: update cubelist for postive cofactor
        posCofacTautology = checkForTautology(myCubeList);
        //TODO: update cubelist for negative cofactor
        negCofacTautology = checkForTautology(myCubeList);
        
        return posCofacTautology && negCofacTautology;
    }
    
    return tautology;
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
    cubeValifOff = numVars;
    cubeUniOff = numVars + 1;

    if ((myCubeList = (CubeList*)malloc(sizeof(CubeList))) == NULL) {
        printf("MALLOC ERROR!\n");
        exit(1);
    }
    myCubeList->numVars = numVars;
    myCubeList->numCubes = numCubes;

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
        if ((myCubeList->cubes[i] = (unsigned char*)calloc(numVars + 2, sizeof(unsigned char))) == NULL) {
            printf("CALLOC ERROR!\n");
            exit(1);
        }
    }

    //parse file and populate cube arrays 
    while (cubeCount < numCubes) {
        int uniCube = 0;
        std::cin >> cubeStr;

        if(debug) printf("input cube string: %s\n", cubeStr);

        //parse cubes and populate cube arrays
        for (int ivar = 0; ivar < numVars; ivar++) {
            if (cubeStr[ivar] == '0') {
                myCubeList->cubes[cubeCount][ivar] = 0;
            } else if (cubeStr[ivar] == '1') {
                myCubeList->cubes[cubeCount][ivar] = 1;
            } else if (cubeStr[ivar] == '-' || cubeStr[ivar] == '_') {
                myCubeList->cubes[cubeCount][ivar] = 2;
                uniCube++;
            } else {
                printf("INVALID CHARACTER IN CUBE\n");
                exit(1);
            }

            myCubeList->cubes[cubeCount][cubeUniOff] +=  myCubeList->cubes[cubeCount][ivar];
        }

        if (uniCube == numVars) {
            printf("COVER IS A TAUTOLOGY\n");
            //TODO: write tautology cover file
            return 0;
        }

        cubeCount++;
    }

    ///parse var colums of cube matrix to populate boolvar info
    for (int ivar = 0; ivar < numVars; ivar++) {
        for (int icube = 0; icube < numCubes; icube++) {
            if(debug) printf("boolvar[%i] in cube[%i] = %i\n", ivar, icube, myCubeList->cubes[icube][ivar]);

            if (myCubeList->cubes[icube][ivar] == 1) { 
                if(debug) printf("\tincrementing numTrue\n");
                myCubeList->vars[ivar]->numTrue++;
            } else if (myCubeList->cubes[icube][ivar] == 0) {
                if(debug) printf("\tincrementing numComp\n");
                myCubeList->vars[ivar]->numComp++;
            } else {
                if(debug) printf("\tincrementing numDC\n");
                myCubeList->vars[ivar]->numDC++;
            }
        }

        //determine whether or not var is single polarity 
        if ( (myCubeList->vars[ivar]->numTrue > 0 && myCubeList->vars[ivar]->numComp == 0) ||
             (myCubeList->vars[ivar]->numComp > 0 && myCubeList->vars[ivar]->numTrue == 0) ||
             (myCubeList->vars[ivar]->numDC == numCubes) )

        { 
            myCubeList->vars[ivar]->polarity = SINGLE;
            myCubeList->vars[ivar]->dependence = -1;
        } else {
            myCubeList->vars[ivar]->polarity = DOUBLE;
            myCubeList->vars[ivar]->dependence = abs(myCubeList->vars[ivar]->numTrue - myCubeList->vars[ivar]->numComp);
        }
    }

    checkForUnateCubeList(myCubeList);

    if (debug) {
        for (int icubes = 0; icubes < numCubes; icubes++) {
            printf("cube[%i]: ", icubes);
            for (int ivars = 0; ivars < numVars; ivars++) {
                if (myCubeList->cubes[icubes][ivars] == 2) {
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
        }
    }

    time_t tf = time(NULL);
    printf("Run time: %i hour(s), %i min(s), %i (secs)\n",
            int((tf - t0)/3600), int(((tf - t0)%3600)/60), int((tf - t0)%60));

    free(myCubeList->vars);
    free(myCubeList->cubes);
    free(myCubeList);

    return 0;
}
