#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main(int argc,char **argv)
{
    int i, j, d;              // Counters for numerus purposes
    char fileName[100];       // The name is pretty accurate
    char tempChar;            // Buffer... stores a character temporarily
    double tempDouble;        // Buffer for doubles
    int dimentions = 1;       // Dimention counter for the file
    int elementCounter = 0;   // Element counter for the file
    int clusterNum;           // Stores the cluster num
    double minDistance;       // This is utilised in the element assignment to clusters
    int minCluster;           // Closest cluster at the moment
    int biggestCluster = 0;   // Cluster containing the most data points, used for memory alocation
    int changesCommited;      // This will be used as a flag to end the algorythm when changes no longer happen
    int iterationCounter = 0; // This counts iterations
    int changesCounter = 0;   // This is used to determine when to terminate the program
    int dbg = 0;              // Debug variable should be deleted printf("Debug %d\n",dbg++);

    // diagnosing file counting inputs
    FILE *f;
    if (argc == 3) {
        // Getting data from main()
        f = fopen(argv[1],"r");
        clusterNum = atoi(argv[2]);
    } else {
        // Getting data from user
        printf("Insert Data file: ");
        scanf("%s", fileName);
        f = fopen(fileName, "r");
        printf("Input the amount of clusters: ");
        scanf("%d", &clusterNum);
    }

    // Reading file
    if (!f) {
        printf("Invalid file\n");
        return 0;
    }
    do {
        fscanf(f, "%c", &tempChar);
        if (tempChar == ' ')
            dimentions++;
    } while (tempChar != '\n');
    rewind(f);
    while (!feof(f))
    {
        fscanf(f, "%c", &tempChar);
        if (tempChar == '\n')
            elementCounter++;
    }
    elementCounter++;
    rewind(f);
    printf("Detected \n Elements : %d\n Dimentions : %d\n", elementCounter, dimentions);
    
    // Array for elements first dim shows id second dimentions
    double **elements;
    elements = calloc(elementCounter,sizeof(double));
    for (i=0; i<elementCounter; i++)
        elements[i] = calloc(dimentions,sizeof(double));

    // Reading and storing elements in an array
    for (i = 0; i < elementCounter; i++)
        for (j = 0; j < dimentions; j++)
            fscanf(f, "%lf", &elements[i][j]);

    
    // Creating Centroid array
    double **centroids;
    centroids = calloc(clusterNum,sizeof(double));
    for (i=0; i<clusterNum; i++)
        centroids[i] = calloc(dimentions,sizeof(double));

    // Generating initial Centroids
    for (i = 0; i < clusterNum; i++)
        for (j = 0; j < dimentions; j++)
            centroids[i][j] = elements[rand() % elementCounter][j];
            // The line above may not correspond with k-means philoshophy

    int *clusterSize;
    int **clusterSum;
    int **clusters;

    int **exClusters;
    exClusters = calloc(clusterNum,sizeof(int));

    int changed[clusterNum];                    // If changes are commited it will resume execution

    do
    {
        // Reseting counters
        biggestCluster = 0;
        changesCommited = 1;

        // Initialising clusters
        printf("Debug %d\n",dbg++);
        clusters = calloc(clusterNum,sizeof(int));

        // Reseting cluster sizes
        clusterSize = calloc(clusterNum,sizeof(int));

        // Storing current cluster sums per dimention
        clusterSum = calloc(clusterNum,sizeof(double));
        for (i = 0; i < clusterNum; i++)
            clusterSum[i] = calloc(dimentions,sizeof(double));

        // Asigning items to Centroids
        for (i = 0; i < elementCounter; i++)
        {
            minDistance = 99999999; //change that
            minCluster = 0;
            for (j = 0; j < clusterNum; j++)
            {
                // Tracking the closest centroid
                tempDouble = 0;
                for (d = 0; d < dimentions; d++)
                    tempDouble += (centroids[j][d] - elements[i][d])*(centroids[j][d] - elements[i][d]);

                tempDouble = sqrt(tempDouble);

                // finding the closest centroid
                if (tempDouble <= minDistance)
                {
                    minDistance = tempDouble;
                    minCluster = j;
                }
            }
            // Storing element in cluster
            clusterSize[minCluster] ++;
            if (clusterSize[minCluster] > biggestCluster)
                for (j=0; j<clusterNum; j++) {
                    clusters[j] = calloc(1,sizeof(int));
                    biggestCluster ++;
                }
            clusters[minCluster][ clusterSize[minCluster] ] = i;

            for (d = 0; d < dimentions; d++) {
                clusterSum[minCluster][d] += elements[ clusters[minCluster][clusterSize[minCluster]] ][d];
            }
            
        }

        // Calculating new centroid
        for (i = 0; i < clusterNum; i++)
            for (j = 0; j < dimentions; j++)
                centroids[i][j] = clusterSum[i][j] / (clusterSize[i]);
        printf("New Centroid is : %lf %lf %lf \n",centroids[0][0],centroids[0][1],centroids[0][2]);
        printf("New Centroid is : %lf %lf %lf \n",centroids[1][0],centroids[1][1],centroids[1][2]);
        
        iterationCounter++;
        printf("Iteration counter %d \n",iterationCounter);

        // Find if it should terminate
        // Termination is wrong system needs to terminate when less than 2% of points change
        if (iterationCounter == 1)
            for(i=0; i<clusterNum; i++)
                exClusters[i] = calloc(biggestCluster,sizeof(int));

        for (i=0; i<clusterNum; i++)
            for (j=0; j<clusterSize[i]; j++) {
                if (exClusters[i][j] != clusters[i][j])
                    changesCounter ++;
                exClusters[i][j] = clusters[i][j];
            }
        printf("Changes %d\n",changesCounter);
        
        // if (changesCounter < elementCounter) //This is why it doesn't stop
        if (iterationCounter >= 50) {
            changesCommited = 0;
        } else {
            // In case it needs to be run again those need to reset
            free(clusterSize);
            free(clusterSum);
            free(clusters);
            printf("cleaning complete\n");
        }
    } while (changesCommited == 1);

    
    fclose(f);

    printf("iterations : %d\n", iterationCounter);
    // Write results in a file
    for (i = 0; i < clusterNum; i++)
    {
        sprintf(fileName, "Cluster%d.csv", i);
        f = fopen(fileName, "w");
        for (j = 0; j < clusterSize[i]; j++)
        {
            for (d = 0; d < dimentions; d++)
                fprintf(f, "%lf ", elements[j][d]);
            fprintf(f, " \n");
        }
        fclose(f);
    }
    return 0;
}