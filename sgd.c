#include "util.h"

void stochasticGD(int k, int d,double gamma, double lambda, DLIST* users, DLIST* movies)
{
    //a is a vector of doubles; so is b.
    //A's size is the number of movies, b is users
    //Compute mu by iterating through all available ratings, summing them up, and dividing by K
    double mu, rhat, r_ui, error;
    int itmax = 10000;
    int itcount = 0;
    int it;//,count ;
    double tol =1;
    double totalerr_new, totalerr;
    //double gamma;// gamma_b ;
    double err; //top_a,bot_a,top_b,bot_b;
    double alpha, beta;
    clock_t t1, t2;
    float ratio;

    ratio = 1./CLOCKS_PER_SEC; //set up ish for timing

    mu = rhat = r_ui = error = totalerr = 0.0;

    t1 = clock(); //start timing

    for (int i = 0; i < movies->length; i++)
    {
        MOVIE* temp = (MOVIE*) dlistGet(movies, i);
        for (int j = 0; j < temp->numRatings; j++)
        {
            mu += temp->ratings[j];
        }
    }
    mu = mu / (k * 1.0);
    totalerr_new = 0;
    // printf("%d total users\n",users->length);
    fflush(stdout);
    while (itcount < itmax)
    {
        itcount++;
        it = 0;
        totalerr = totalerr_new;
        totalerr_new = 0;
        err = 0;
        int chk = 0;
        int userIndex = 0;
        int spent[users->length];
        //srand(itcount*17*17*17);
        //memset(spent,-1,(users->length)*sizeof(int));
        //      printf("memset success\n");
        //for (int u = 0; u < users->length; u++)

        /*while (chk < users->length)
        {
         while (contains(spent,users->length,userIndex) == 1)
         {
             userIndex = (rand() % (users->length));
         }
         int u = userIndex;
         spent[chk++] = u;
         //printf("Using user index  = %d \n",u);
         fflush(stdout);
         */
        for (int u = 0; u < users->length; u++)
        {
            USER* user = ((USER*)dlistGet(users, u));
            for (int i = 0; i < user->numMovies; i++)
            {
                MOVIE* movie = user->movies[i];
                r_ui = user->ratings[i];
                //rhat=mu +user->b +movie->a;
                //  printf("value: %lf\n",dotProduct(user->p,movie->q,d));
                rhat = mu + user->b + movie->a+dotProduct(user->p,movie->q,d);
                /*for (int z = 0; z < d; ++z)
              {
                   printf("p[%d] = %lf\tq[%d] = %lf\n",z,user->p[z],z,movie->q[z]);
                }*/
                error = r_ui - rhat;
                totalerr_new += (error * error);
                err += fabs(error);

                //update equations for the a and b scalar parameters.

                movie->a -= gamma/(movie->numRatings) * (lambda * movie->a - error);
                user->b -= gamma/(user->numMovies) * (lambda * user->b - error);

                // movie->a-=gamma*(lambda*movie->a-error);
                // user->b-= gamma*(lambda*user->b-error);
                //Update equations for the p and q vectors, element by element
                for (int i=0;i<d;i++)
                {
                    movie->q[i] -=  gamma/(movie->numRatings)*(lambda*movie->q[i]-user->p[i]*error);
                    user->p[i] -=   gamma/(user->numMovies)*(lambda*user->p[i]-movie->q[i]*error);
                }
		/*     if ((itcount%25==0) && (gamma>.005))
                    {
                        //  count++;
                        gamma=gamma/(1.3);
                        // printf("Current count: %d\n",count);
	        	}  */
                it++;
            }
        }

        t2 = clock(); //end timing

        //alpha error from past iteration
        alpha = totalerr;  /* itcount*1.0;*/
        beta = totalerr_new; /* (itcount+1.0);*/
        //printf("MSE = %lf \n", beta/k);
        /*  if (itcount%5==0)
        printf("Current MSE: %lf \t Current Iter: %d\n",beta/it,itcount);
        */
        //exit loop is error over last change did not improve substantially or the error is blowing up
        if (((alpha - beta) < tol && itcount >10)  || ((beta/k)>2  && itcount>10))
        {
            if ((beta/k)>2)
            {
                printf("Error Not Converging. Test Exited Early\n");
            }
            break;
        }
        // printf("%d\n", itcount);
    }

//    MOVIE* temp = (MOVIE*) dlistGet(movies, 10);
//  printf("a: %8.6f\n", temp->a);
// printf("b: %8.6f\n", temp->users[2]->b);
//printf("pq: %8.6f\n", dotprod(temp->q, temp->users[2]->p, d));
    //printf("-----------------------\n");
    //  printf("%lf\t %lf\t %d\t",gamma,lambda,itcount);
    printf("Iterations = %d \n", itcount);
    printf("Train Error = %lf \n", beta/k);
    //printf("alpha = %lf \n", alpha);
    //printf("mu = %lf \n", mu);
    printf("Users = %d \n", users->length);
    printf("Movies = %d \n", movies->length);
//printf("numObs= %d \n", it);
    printf("Vector length=  %d  \n",d);
    printf("Gamma= %lf   \n", gamma);
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
    return;
}

double dotprod(double *p,double *q,int n)
{
    double sum=0.0;
    if (n>0)
    {
        for (int i=0;i<n;i++)
        {
            sum+= p[i]*q[i];
        }
        return sum;
    }
    else {
        return 0.0;
    }
}

// kate: indent-mode cstyle; space-indent on; indent-width 0; 
