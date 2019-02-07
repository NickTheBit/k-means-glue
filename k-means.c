#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
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
    int changesCommited;      // This will be used as a flag to end the algorythm when changes no longer happen
    int iterationCounter = 0; // This counts iterations
    int dbg = 0;              // Debug variable should be deleted printf("Debug %d",dbg++);

    // diagnosing file counting inputs
    FILE *f;
    printf("Insert Data file: ");
    scanf("%s", fileName);
    f = fopen(fileName, "r");
    if (!f)
    {
        printf("Invalid file\n");
        return 0;
    }
    do
    {
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

    // Requesting number of clusters this will need to change
    printf("Input the amount of clusters: ");
    scanf("%d", &clusterNum);

    struct clusterData
    {
        int pointNum;
        int *points;
        double clusterSum[dimentions];
    };

    // Creating Centroid array
    double centroids[clusterNum][dimentions];

    // Generating initial Centroids
    for (i = 0; i < clusterNum; i++)
        for (j = 0; j < dimentions; j++)
        {
            centroids[i][j] = elements[rand() % elementCounter][j];
            // The line above may not correspond with k-means philoshophy
        }

    // Initialising cluster arrays thats wrong
    // int clusters[clusterNum][elementCounter];

    // Testing ne technique
    struct clusterData clusters[clusterNum];

    int changed[clusterNum];                    // If changes are commited it will resume execution
    double exCentroids[clusterNum][dimentions]; // Var for locating changes in centroids

    do
    {
        // Reinitialising cluster arrays thats wrong
        // int clusters[clusterNum][elementCounter];
        struct clusterData clusters[clusterNum];

        for (i = 0; i < clusterNum; i++)
        {
            clusters[i].pointNum = 0;
            changed[i] = 1;
            for (j = 0; j < dimentions; j++)
                clusters[i].clusterSum[j] = 0;
        }

        // asigning items to Centroids
        for (i = 0; i < elementCounter; i++)
        {
            minDistance = 99999999; //change that
            minCluster = 0;
            for (j = 0; j < clusterNum; j++)
            {
                // avg of dimention values may be wrong
                tempDouble = 0;
                for (d = 0; d < dimentions; d++)
                    tempDouble += abs(centroids[j][d] - elements[i][d]);

                // finding the closest centroid
                if ((tempDouble / dimentions) <= minDistance)
                {
                    minDistance = (tempDouble / dimentions);
                    minCluster = j;
                }
            }

            // Possible error creating space for element then storing it
            clusters[minCluster].points = malloc(sizeof(int)); //dont forget realloc
            clusters[minCluster].points[clusters[minCluster].pointNum] = i;
            for (d = 0; d < dimentions; d++)
                clusters[minCluster].clusterSum[d] += elements[clusters[minCluster].pointNum][d];
            clusters[minCluster].pointNum++;
        }
        // Calculating new centroid
        for (i = 0; i < clusterNum; i++)
            for (j = 0; j < dimentions; j++)
            {
                centroids[i][j] = clusters[i].clusterSum[j] / clusters[i].pointNum;
                if (centroids[i][j] == exCentroids[i][j])
                    changed[i] = 0;
                exCentroids[i][j] = centroids[i][j];
            }
        // tempDouble = tempDouble/(clusterPoints[i]/dimentions);

        iterationCounter++;

        // Find if it should terminate
        changesCommited = 0;
        for (i = 0; i < clusterNum; i++)
            if (changed[i] != 0)
                changesCommited = 1;

        // It runs twice for the first element

    } while (changesCommited == 1);

    fclose(f);

    printf("iterations : %d\n", iterationCounter);
    // Write results in a file
    for (i = 0; i < clusterNum; i++)
    {
        sprintf(fileName, "Cluster%d.csv", i);
        f = fopen(fileName, "w");
        for (j = 0; j < clusters[i].pointNum; j++)
        {
            for (d = 0; d < dimentions; d++)
                fprintf(f, "%lf ", elements[clusters[i].points[j]][d]);
            fprintf(f, "\n");
        }
        fclose(f);
    }

    return 0;
}