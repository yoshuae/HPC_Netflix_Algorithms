#include "util.h"
int readInputFile(FILE* ifp, DLIST* users, DLIST* movies, int d, const char* rule)
{

    int tempMovieID, tempUserID, count, totalMovies, totalUsers;
    double tempMovieRating;
    totalMovies = 0;
    totalUsers = 0;
    count = 0;
    double trash;
    //Read the file and get the movie id, user id, and rating
    while (fscanf(ifp, rule, &tempUserID, &tempMovieID, &tempMovieRating,&trash) != EOF)
    {
      //printf("The line contained: %d %d %lf %lf\n",tempUserID,tempMovieID,tempMovieRating,trash);
        //We do not want repeat user id's, repeat movie id's are fine
        int iID = dlistContains(movies, (void*) & tempMovieID);
        int uID = dlistContains(users, (void*) & tempUserID);
        //movie list does not contain tempMovieID
        if (iID == -1)
        {
            totalMovies++;
            MOVIE* tempMovie = (MOVIE*) malloc(sizeof(MOVIE));
            tempMovie->id = tempMovieID;
            tempMovie->numRatings = 1;
            tempMovie->ratings = (double*) malloc(sizeof(double));
            tempMovie->users = (USER**) malloc(sizeof(USER*));
            tempMovie->q = (double*) malloc(sizeof(double) *d);
            for (int k = 0; k < d; k++)
            {
                double part1 = ((rand() % 9) + 1);
                double part2 = ((rand() % 100000) + 1) / 10000.0;
                double part3 = ((rand() % 177) + 1) / 149.0;
                double part4 = ((rand() % 1000) + 1) / 643.0;
                tempMovie->q[k] = part1 + (0.1) * part2 + (0.00001) * part3 + (0.000000000001) * part4;
            }
            tempMovie->ratings[0] = tempMovieRating;
            tempMovie->a = 0.0;
            dlistAdd(movies, tempMovie);
        }
        else
        {
            MOVIE* tempMovie = (MOVIE*) dlistGet(movies, iID);
            tempMovie->numRatings++;
            tempMovie->ratings = (double*)
                                 realloc(tempMovie->ratings, tempMovie->numRatings * sizeof(double));
            tempMovie->ratings[tempMovie->numRatings-1] = tempMovieRating;
            tempMovie->users = (USER**)
                               realloc(tempMovie->users, tempMovie->numRatings * sizeof(USER));

        }
        //user list does not contain tempUserID
        if (uID == -1)
        {
            totalUsers++;
            USER* tempUser = (USER*) malloc(sizeof(USER));
            tempUser->id = tempUserID;
            tempUser->numMovies = 1;
            tempUser->movies = (MOVIE**) malloc(sizeof(MOVIE*));
            tempUser->ratings = (double*) malloc(sizeof(double));
            tempUser->p = (double*) calloc(d,sizeof(double));
            tempUser->movies[0] = ((MOVIE*) dlistGet(movies, dlistContains(movies, (void*) & tempMovieID)));
            tempUser->ratings[0] = tempMovieRating;
            tempUser->b = 0.0;
            dlistAdd(users, tempUser);
        }
        else
        {
            USER* tempUser = (USER*) dlistGet(users, uID);
            tempUser->numMovies++;
            tempUser->movies = (MOVIE**) realloc(tempUser->movies, tempUser->numMovies * sizeof(MOVIE*));
            tempUser->movies[tempUser->numMovies-1] = ((MOVIE*) dlistGet(movies, dlistContains(movies, (void*) & tempMovieID)));
            tempUser->ratings = (double*) realloc(tempUser->ratings, tempUser->numMovies * sizeof(double));
            tempUser->ratings[tempUser->numMovies-1] = tempMovieRating;
        }
        //Properly link the movie with its user
        MOVIE* temp = (MOVIE*) dlistGet(movies, dlistContains(movies, (void*) & tempMovieID));
        temp->users[temp->numRatings-1] = ((USER*) dlistGet(users, dlistContains(users, (void*) &tempUserID)));

        //fprintf(stdout,"Iterated thru %d lines\n",count);
        count++;
    }

    fclose(ifp);
    return count;
}

void initMPIStructs()
{
  //This is the number of variables in the structure
  int blocks = 6;
  int lengths[blocks];
  MPI_Aint disp[blocks];
  MPI_Datatype types[blocks];
  lengths[0] = 1;
  lengths[1] = 1;
  lengths[2] = 1;
  lengths[3] = 1;
  lengths[4] = 1;
  lengths[5] = 1;
  disp[0] = 0;
  disp[1] = 4;
  disp[2] = 8;
  disp[3] = 16;
  disp[4] = 24;
  disp[5] = 32;
  types[0] = MPI_INT;
  types[1] = MPI_INT;
  types[2] = MPI_UNSIGNED_LONG;
  types[3] = MPI_DOUBLE;
  types[4] = MPI_DOUBLE;
  types[5] = MPI_DOUBLE;
  MPI_Type_create_struct(blocks,lengths,disp,types,&MPI_USER);
  MPI_Type_create_struct(blocks,lengths,disp,types,&MPI_ITEM);
  MPI_Type_commit(&MPI_USER);
  MPI_Type_commit(&MPI_ITEM);
}

double dotProduct (double *x, double *y, int n)
{
   double dp;
   int i;

   //#ifdef BLAS
   //dp = ddot(n, x, 1, y, 1);
   //#else
   dp = 0.0;
   for (i = 0; i < n; i++)
      dp += x[i] * y[i];
   //#endif

   return dp;
}
int contains(int vector[], int n, int query)
{
   for (int i = 0; i < n; ++i)
   {
      if(vector[i] == query)
      {
         return 1;
      }
   }
   return 0;
}
