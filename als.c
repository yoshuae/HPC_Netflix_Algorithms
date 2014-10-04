#include "util.h"

void solvep(int d, double lambda, double*** xq, double** yUsers, DLIST* users, DLIST* movies)
{
    int info;
    int ipiv[d*d];
    double row[d];
    double** temp;
    double phat[d];
    double* xprimex;

    for (int u = 0; u < users->length; u++)
    {
        USER tempUser = *((USER*) dlistGet(users, u));

        xprimex = (double*) malloc(sizeof(double) * d*d);
        /*
        for(int i = 0; i < d; i++)
        {
          xprimex[i] = (double*) malloc(sizeof(double) * d);
        }*/


        //Create xprimex matrix (xq' * xq + lambdaI)
        for (int p = 0; p < tempUser.numMovies; p++)
        {
            for (int k = 0; k < d; k++)
            {
                for (int j = 0; j < d; j++)
                {
                    xprimex[k+(d*j)] += xq[u][p][k]*xq[u][p][j];
                    if (k==j)
                    {
                        xprimex[k+(d*j)] += lambda;
                    }
                }
            }
        }
        //invert xprimex
        dgetrf(d,d,xprimex,d,ipiv,&info);
        dgetri(d,xprimex,d,ipiv,&info);

        //multiply inverse by xq'
        temp = (double**) malloc(sizeof(double) *d);
        for (int i = 0; i < d; i++)
        {
            temp[i] = (double*) malloc(sizeof(double) * tempUser.numMovies);
        }

        for (int j = 0; j < d; j++)
        {

            for (int k = 0; k < d; k++)
            {
                row[k] = xprimex[j+(d*k)];
            }

            for (int k = 0; k < tempUser.numMovies; k++)
            {
                temp[j][k] = dotProduct(row, xq[u][k], d);
            }
        }

        //multiply temp by y_u to get estimate for p.
        for (int j = 0; j < d; j++)
        {
            phat[j] = dotProduct(temp[j], yUsers[u], tempUser.numMovies);
        }


        tempUser.p = phat;


        //Freeing variables
        for (int i = 0; i < d; i++)
        {
            free(temp[i]);
            //           free(xprimex[i]);
        }

        free(xprimex);
        free(temp);
        for (int i = 0; i < d; i++)
            ((USER*)dlistGet(users,u))->p[i] = tempUser.p[i];
    }

}
void solveq(int d, double lambda, double*** xp, double** yItems, DLIST* users, DLIST* movies)
{
    int info;
    int ipiv[d*d];
    double** temp;
    double row[d];
    double* xprimex;
    double qhat[d];


    for (int u = 0; u < movies->length; u++)
    {
        MOVIE tempMovie = *((MOVIE*) dlistGet(movies, u));
        xprimex = (double*) malloc(sizeof(double) *d*d);

        /*
        for(int i = 0; i < d; i++)
        {
          xprimex[i] = (double*) malloc(sizeof(double) * d);
        }*/

        //Create xprimex matrix (xp' *xp + lambdaI)
        for (int p = 0; p < tempMovie.numRatings; p++)
        {
            for (int k = 0; k < d; k++)
            {
                for (int j = 0; j < d; j++)
                {
                    xprimex[k+(d*j)] += xp[u][p][k]*xp[u][p][j];
                    if (k==j)
                    {
                        xprimex[k+(d*j)] += lambda;
                    }
                }
            }
        }

        //invert xprimex
        dgetrf(d,d,xprimex,d,ipiv,&info);
        dgetri(d,xprimex,d,ipiv,&info);

        //multiply inverse by xp'
        temp = (double**) malloc(sizeof(double) *d);
        for (int i = 0; i < d; i++)
        {
            temp[i] = (double*) malloc(sizeof(double) * tempMovie.numRatings);
        }

        for (int j = 0; j < d; j++)
        {

            for (int k = 0; k < d; k++)
            {
                row[k] = xprimex[j+(d*k)];
            }

            for (int k = 0; k < tempMovie.numRatings; k++)
            {
                temp[j][k] = dotProduct(row, xp[u][k], d);
            }
        }

        //multiply temp by y_i to get an estimate for q.
        for (int j = 0; j < d; j++)
        {
            qhat[j] = dotProduct(temp[j], yItems[u], tempMovie.numRatings);
        }



        tempMovie.q = qhat;



        //Freeing variables
        for (int i = 0; i < d; i++)
        {
            free(temp[i]);
            //free(xprimex[i]);
        }
        free(xprimex);
        free(temp);
        for (int i = 0; i < d; i++)
            ((MOVIE*)dlistGet(movies,u))->q[i] = tempMovie.q[i];
    }
}


void alternateLS(int k, int d, double lambda, double tol, int itmax, DLIST* users, DLIST* movies)
{
    double **yUsers, **yItems;
    double **xprimex;
    double ***xq, ***xp;
    double MSE, MSEold;
    int it = 0;
    double step;
    //yUsers  is a vector of doubles; so is yItems.
    //The size of yUsers is the number of users;  The size of yItems is the number of movies.
    //xprimex is an array the size of d*d.
    //xq is ...
    //MSE is the mean squared error.
    //it is the iteration count.
    //step is...
    double mu = 0.0;
    MSEold = 1.0;
    MSE = 0.0;
    double muold;
    double norm = 0.0;
    double normp, normq;
    double c;
    clock_t t1, t2;
    float ratio;

    ratio = 1./CLOCKS_PER_SEC; //set up ish for timing

    //Set up vectors yUsers and yItems from the data structure.
    //Set up vectors xq and xp by looping through each user or item and taking the q or p vector fromthe data structure respectivly.
    yUsers = (double**) malloc(sizeof(double*) * users->length);
    yItems = (double**) malloc(sizeof(double*) * movies->length);

    xq = (double***) malloc(sizeof(double**) * users->length);
    xp = (double***) malloc(sizeof(double**) * movies->length);
    
    printf("d = %d\n",d);
    printf("lambda = %4.3f\n", lambda);

    for (int u = 0; u < users->length; u++)
    {
        USER* temp = (USER*)dlistGet(users, u);
        xq[u] = (double**) malloc(sizeof(double*) * temp->numMovies);
    }
    for (int i = 0; i < movies->length; i++)
    {
        MOVIE* temp = (MOVIE*)dlistGet(movies, i);
        xp[i] = (double**) malloc(sizeof(double*) * temp->numRatings);
    }

    //Compute mu by iterating through all available ratings, summing them up, and dividing by K
    for (int i = 0; i < movies->length; i++)
    {
        MOVIE* temp = (MOVIE*) dlistGet(movies, i);
        for (int j = 0; j < temp->numRatings; j++)
        {
            mu += temp->ratings[j];
        }
    }
    mu = mu / (k * 1.0);
   // printf("The initial value of mu %lf \n", mu);
    muold = mu;
    //Compute a_i
    //scan through yItems, take ratings for each movie in yItems, subtract mu from them, and average;
    //store in a_i[index of movie in yItems];

    for (int i = 0; i < movies->length; i++)
    {
        MOVIE* temp = (MOVIE*) dlistGet(movies, i);
        int numRat = temp->numRatings;
        for (int k = 0; k < numRat; k++)
        {
            temp->a += (temp->ratings[k] - mu);
        }
        temp->a = temp->a / (numRat*1.0);
        /* if (i=1){
        for (i=0, i<num
        printf("The value of a_1 is %d" , );
        } */
    }

    //Compute b_u
    //scan through yUsers, take ratings for each user in yUsers, subtract mu from them, and average;
    //store in b_u[index of user in yUsers];

    for (int u = 0; u < users->length; u++)
    {
        USER* temp = (USER*) dlistGet(users, u);
        int numMov = temp->numMovies;
        for (int k = 0; k < numMov; k++)
        {
            temp->b += (temp->ratings[k] - mu);
        }
        temp->b = temp->b / (numMov*1.0);
        //if (u==0) {
           // printf("B = %8.6f \n", temp->b);
        //}
    }

    //Compute initial guess for y_i
    //y[index of movie] is a vector of length numRatings that holds the adjusted ratings each movie received.
    for (int i = 0; i < movies-> length; i++)
    {
        MOVIE* temp = (MOVIE*) dlistGet(movies, i);
        yItems[i] = (double*) malloc(sizeof(double) * temp->numRatings);
        for (int k = 0; k < temp->numRatings; k++)
        {
            yItems[i][k] = temp->ratings[k] - mu - temp->a - temp->users[k]->b;
            //printf("y_i[%d][%d] = %lf\n",i,j,y_i[i][j]);
            //printf("Movie %d was rated %lf by user %d \n", temp->id, temp->ratings[k], temp->users[k]->id);
            //printf("Corresponding q[%d][%d] = %lf\n",i,k,qItems[i][k]);
        }
    }

    //Compute initial guess for y_u.
    //y[index of user] is a vector of length numMovies that holds the sdjusted ratings each user gave.
    for (int u = 0; u < users->length; u++)
    {
        USER* temp = ((USER*) dlistGet(users, u));
        yUsers[u] = (double*) malloc(sizeof(double) * temp->numMovies);
        for (int k = 0; k < temp->numMovies; k++)
        {
            yUsers[u][k] = temp->ratings[k] - mu - temp->b - temp->movies[k]->a;
            //printf("y_u[%d][%d] = %lf\n",i,j,y_u[i][j]);
            //printf("User %d rated movie %d %lf\n", temp->id, temp->movies[k]->id, temp->ratings[k]);

        }
    }
   // printf("\n");
    tol = 0.0001;
    itmax = 100;
    int tempd = d;
    t1 = clock(); //start timing
    //Begin ALS loop, which iteratively solves for q and p.
    while (fabs(MSE - MSEold) > tol && it < itmax)
    {
        // if(it == 0 || it == 1)
        //d=0;
        //  else
        d = tempd;

        if (d>0)
        {
            //Create xq vectors.
            for (int u = 0; u < users->length; u++)
            {
                USER* tempUser = ((USER*) dlistGet(users, u));
                for (int k = 0; k < tempUser->numMovies; k++)
                {
                    xq[u][k] = tempUser->movies[k]->q;
                }

            }


            //Gauss Markov theorem solving for p
            solvep(d, lambda, xq, yUsers, users,  movies);



            //Create xp vectors.
            for (int i = 0; i < movies->length; i++)
            {
                MOVIE* tempMovie = ((MOVIE*) dlistGet(movies, i));

                for (int k = 0; k < tempMovie->numRatings; k++)
                {

                    xp[i][k] = tempMovie->users[k]->p;
                }
            }

            //Gauss-Markov theorm solving for q
            solveq(d, lambda, xp, yItems, users, movies);

        }
        /*
              //Rescale p's and q's
              if(it%2==0)
              {
               for (int u = 0; u < users->length; u++)
                {
                    USER* tempUser = ((USER*) dlistGet(users, u));
             if(u==0)
             norm = sqrt(dot_product(tempUser->p,tempUser->p,d));
                    for (int k = 0; k < d; k++)
                    {
                        tempUser->p[k] = tempUser->p[k] / norm;
                    }

                }

                for (int u = 0; u < movies->length; u++)
                {
                    MOVIE* tempMovie = ((MOVIE*) dlistGet(movies, u));
                    for (int k = 0; k < d; k++)
                    {
                        tempMovie->q[k] = tempMovie->q[k] * norm;
                    }

                }
              }
              else
              {
         for (int u = 0; u < movies->length; u++)
                {
                    MOVIE* tempMovie = ((MOVIE*) dlistGet(movies, u));
             if(u==0)
             norm = sqrt(dot_product(tempMovie->q,tempMovie->q,d));
                    for (int k = 0; k < d; k++)
                    {
                        tempMovie->q[k] = tempMovie->q[k] / norm;
                    }

                }

              for (int u = 0; u < users->length; u++)
                {
                    USER* tempUser = ((USER*) dlistGet(users, u));

                    for (int k = 0; k < d; k++)
                    {
                        tempUser->p[k] = tempUser->p[k] * norm;
                    }

                }


              }
              */

        normp = 0;
        normq = 0;

        for (int u = 0; u < users->length; u++)
        {
            USER* tempUser = ((USER*) dlistGet(users, u));
            normp += dotProduct(tempUser->p,tempUser->p,d);
        }

        for (int u = 0; u < movies->length; u++)
        {
            MOVIE* tempMovie = ((MOVIE*) dlistGet(movies, u));
            normq += dotProduct(tempMovie->q,tempMovie->q,d);
        }

        c = pow(normq/normp, 0.25);
       // printf("c: %7.6f\n", c);

        for (int u = 0; u < users->length; u++)
        {
            USER* tempUser = ((USER*) dlistGet(users, u));
            for (int k = 0; k < d; k++)
            {
                tempUser->p[k] = tempUser->p[k] * c;
            }
        }

        for (int u = 0; u < movies->length; u++)
        {
            MOVIE* tempMovie = ((MOVIE*) dlistGet(movies, u));
            for (int k = 0; k < d; k++)
            {
                tempMovie->q[k] = tempMovie->q[k] / c;
            }

        }


        //update equations
        //for mu
        mu = 0.0;
        for (int i = 0; i < movies->length; i++)
        {
            MOVIE* temp = (MOVIE*) dlistGet(movies, i);
            for (int j = 0; j < temp->numRatings; j++)
            {
                mu += temp->ratings[j] - temp->a - temp->users[j]->b - dotProduct(temp->q, temp->users[j]->p, d);
          /*      if (i==10 & j==2) {
                    //printf("rating: %8.6f\n",);
                    printf("a: %8.6f\n", temp->a);
                    printf("b: %8.6f\n", temp->users[j]->b);
                    printf("pq: %8.6f\n", dotProduct(temp->q, temp->users[j]->p, d));
                    printf("p: ");
                    for (int i = 0; i < d; i++)
                    {
                        printf("%7.6f ", temp->users[j]->p[i]);
                    }
                    printf("\n");
                    printf("q: ");
                    for (int i = 0; i < d; i++)
                    {
                        printf("%7.6f ", temp->q[i]);
                    }
                    printf("\n");
                }
                */
            }
        }
        mu = mu / (k * 1.0);
       // printf("The updated value of mu is %lf \n" , mu);


        //for a_i
        for (int i = 0; i < movies->length; i++)
        {
            MOVIE* temp = (MOVIE*) dlistGet(movies, i);
            //temp->aold = temp->a;
            temp->a = 0.0;
            for (int j = 0; j < temp->numRatings; j++)
            {
                temp->a += temp->ratings[j] - mu - temp->users[j]->b - dotProduct(temp->q, temp->users[j]->p, d);
            }
            temp->a = temp->a / (temp->numRatings * 1.0);
        }


        //for b_u
        for (int i = 0; i < users->length; i++)
        {
            USER* temp = (USER*) dlistGet(users, i);
            temp->b = 0.0;
            for (int j = 0; j < temp->numMovies; j++)
            {
                temp->b += temp->ratings[j] - mu - temp->movies[j]->a /*old*/ - dotProduct(temp->p, temp->movies[j]->q, d);
            }
            temp->b = temp->b / (temp->numMovies * 1.0);
        }

        //for y_i
        for (int i = 0; i < movies->length; i++)
        {
            MOVIE* temp = (MOVIE*) dlistGet(movies, i);
            for (int j = 0; j < temp->numRatings; j++)
            {
                yItems[i][j] = temp->ratings[j] - mu - temp->a - temp->users[j]->b;
            }
        }

        //for y_u
        for (int i = 0; i < users->length; i++)
        {
            USER* temp = (USER*) dlistGet(users, i);
            for (int j = 0; j < temp->numMovies; j++)
            {
                yUsers[i][j] = temp->ratings[j] - mu - temp->b - temp->movies[j]->a;
            }
        }

        //Compute the MSE

        MSEold = MSE;
        MSE = 0.0;

        for (int i = 0; i < users->length; i++)
        {
            USER* temp = (USER*) dlistGet(users, i);
            for (int j = 0; j < temp->numMovies; j++)
            {
                step = temp->ratings[j] - mu - temp->movies[j]->a - temp->b - dotProduct(temp->p, temp->movies[j]->q, d);
                MSE += (step*step);
            }
        }

        MSE = MSE / (k*1.0);

      //  printf("MSE = %lf for iteration %d \n", MSE, it);


        it++;
        muold = mu;
        //mu = 0.0;

    }
    t2 = clock(); //end timing

    printf("Iterations = %d \n", it);
    printf("Train Error = %lf \n", MSE);
    //printf("alpha = %lf \n", alpha);
    //printf("mu = %lf \n", mu);
    printf("Users = %d \n", users->length);
    printf("Movies = %d \n", movies->length);
    //printf("numObs= %d \n", it);
    printf("Vector length=  %d  \n",d);
    //printf("gamma= %lf   \n", gamma);
    printf("Lambda= %lf   \n",lambda);
    printf("Time in sec= %f\n",ratio*(long)t1 + ratio*(long)t2);
    printf("--------------------------\n");
    for (int u = 0; u < users->length; u++)
    {
      USER* user = ((USER*)dlistGet(users, u));
      printf("User [%5.5d]:",user->id);
      printf("[");
      for (int i = 0; i < d; i++)
          printf("%lf ",user->p[i]);
      printf("]\n");
    }
    /* Test output for xq and yuser
    int probe =1;
    printf("xq[%d]\n",probe);

    USER* tempUser = ((USER*) dlistGet(users,probe));
    for (int k = 0; k < tempUser->numMovies; k++)
    {
       printf("xq[%d][%d]'s id = %d\n",probe,k,tempUser->movies[k]->id);
       for (int z = 0; z < tempUser->numMovies; z++)
          printf("%lf\n",xq[probe][k][z]);
    }

    for (int u = 0; u < tempUser->numMovies; u++)
    {
    printf("yUser[%d][%d]'s id = %d\n",probe,u,tempUser->movies[u]->id);
    for (int z = 0; z < tempUser->numMovies; z++)
       printf("%lf\n",tempUser->movies[u]->q[z]);
    }*/


    for (int u = 0; u < users->length; u++)
    {
        free(xq[u]);
    }
    free(xq);

    for (int u = 0; u < movies->length; u++)
    {
        free(xp[u]);
    }
    free(xp);

    for (int i = 0; i < movies->length; i++)
        free(yItems[i]);
    for (int u = 0; u < users->length; u++)
        free(yUsers[u]);
    free(yUsers);
    free(yItems);
}




// kate: indent-mode cstyle; space-indent on; indent-width 0; 
