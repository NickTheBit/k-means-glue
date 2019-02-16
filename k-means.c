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
    int minCluster;           // Closest cluster at the current centroid
    int biggestCluster = 0;   // Cluster containing the most data points, used for memory alocation
    int changesCommited;      // This will be used as a flag to end the algorythm when changes no longer happen
    int iterationCounter = 0; // This counts iterations
    int ready[clusterNum];    // Ceeps record of centroid changes for termination

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

    // Creating Centroid array and secondary array for termination check
    double **centroids;
    double **exCentroids;

    centroids = calloc(clusterNum,sizeof(double));
    exCentroids = calloc(clusterNum, sizeof(double));

    for (i=0; i<clusterNum; i++) {
        centroids[i] = calloc(dimentions,sizeof(double));
        exCentroids[i] = calloc(dimentions, sizeof(double));
    }

    // Generating initial Centroids
    for (i = 0; i < clusterNum; i++)
        for (j = 0; j < dimentions; j++)
            centroids[i][j] = elements[rand() % elementCounter][j];
            // The line above may not correspond with k-means philoshophy

    int *clusterSize;
    clusterSize = calloc(clusterNum,sizeof(int));
    
    int **clusterSum;
    clusterSum = calloc(clusterNum,sizeof(double));
    for (i = 0; i < clusterNum; i++)
        clusterSum[i] = calloc(dimentions,sizeof(double));

    int **clusters;
    clusters = calloc(clusterNum,sizeof(int));

    // Starting loop for iterations
    do {

        for (i=0; i<clusterNum; i++)
            ready[i] = 0;

        // Asigning items to Centroids
        for (i = 0; i < elementCounter; i++)
        {
            minDistance = 99999999; //change that... or not, its the biggest possible number...
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
            // Storing element in cluster PROBLEMA
            clusterSize[minCluster] ++;
            if (clusterSize[minCluster] > biggestCluster)
                for (j=0; j<clusterNum; j++) {
                    clusters[j] = calloc(1,sizeof(int));
                    biggestCluster ++;
                }
            clusters[minCluster][ clusterSize[minCluster] ] = i;
            for (d = 0; d < dimentions; d++)
                clusterSum[minCluster][d] += elements[ clusters[minCluster][clusterSize[minCluster]] ][d];
        }
        // Calculating new centroid
        for (i = 0; i < clusterNum; i++)
            for (j = 0; j < dimentions; j++)
                centroids[i][j] = clusterSum[i][j] / (clusterSize[i]);

        iterationCounter++;

        // Find if it should terminate
        for (i=0; i<clusterNum; i++) 
            for (j=0; j<dimentions; j++) {
                if (centroids[i][j] == exCentroids[i][j]) {
                    ready[i] = 1;
                } else {
                    ready[i] = 0;
                }
                // Alocating centroids for next termination check
                exCentroids[i][j] = centroids[i][j];
            }

        changesCommited = 0;
        for (i=0; i<clusterNum; i++)
            if (ready[i] == 0)
                changesCommited = 1;

        // In case it needs to be run again those need to reset
        if (changesCommited == 1) {
            for (i=0; i<clusterNum; i++) {
                memset(clusters[i], 0,dimentions*sizeof(int));
                memset(clusterSum[i], 0,dimentions*sizeof(double));
            }
            // Reseting cluster sizes
            memset(clusterSize, 0,clusterNum*sizeof(int));
        }
    } while (changesCommited == 1);

    printf("Iterations : %d\n", iterationCounter);
    fclose(f);
    // Write results in a file
    for (i = 0; i<clusterNum; i++)
    {
        sprintf(fileName, "Cluster%d.csv", i);
        f = fopen(fileName, "w");
        for (j = 1; j <= clusterSize[i]; j++)
        {
            for (d = 0; d < dimentions; d++)
                fprintf(f, "%lf ", elements[clusters[i][j]][d]);
            fprintf(f, " \n");
        }
        fclose(f);
    } 
    return 0;
}