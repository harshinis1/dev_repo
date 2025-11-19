#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define MAX_CANDIDATES 20
#define MAX_NAME_LENGTH 21
#define MAX_VOTERS 999

///////////////////////////////////////////////////////////////////////

int getword(char W[], int limit);
void blank_line(void);
void blank_space(void);

void stage1(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int *num_candidates,
            int *num_votes);

void count_first_pref(int counts[],
                      int votes[][MAX_CANDIDATES],
                      int eliminated[],
                      int num_candidates,
                      int num_votes);

int find_lowest_candidate(int counts[],
                          int eliminated[],
                          int num_candidates);

void redistribute_votes(int elim_index,
                        int votes[][MAX_CANDIDATES],
                        int num_candidates,
                        int num_votes);

void print_unsorted_round(int round, int counts[],
                          char cand[][MAX_NAME_LENGTH],
                          int eliminated[],
                          int num_candidates);

int compare_candidates(int a, int b, int counts[]);
void insertion_sort_indices(int indices[], int n, int counts[]);
void print_sorted_round(int round, int counts[],
                        char cand[][MAX_NAME_LENGTH],
                        int eliminated[],
                        int num_candidates);

void run_elimination_round(int sorted,
                           char cand[][MAX_NAME_LENGTH],
                           int votes[][MAX_CANDIDATES],
                           int eliminated[],
                           int num_candidates,
                           int num_votes);

void stage2(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int num_candidates,
            int num_votes);

void stage3(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int num_candidates,
            int num_votes);

///////////////////////////////////////////////////////////////////////

// main program provides traffic control
int main(int argc, char *argv[]) {
    char cand[MAX_CANDIDATES][MAX_NAME_LENGTH];
    int votes[MAX_VOTERS][MAX_CANDIDATES];
    int original_votes[MAX_VOTERS][MAX_CANDIDATES];
    int eliminated[MAX_CANDIDATES] = {0};
    int num_candidates = 0, num_votes = 0;

    stage1(cand,votes,original_votes,eliminated,&num_candidates,&num_votes);
    stage2(cand,votes,original_votes,eliminated,num_candidates,num_votes);
    blank_space();
    stage3(cand,votes,original_votes,eliminated,num_candidates,num_votes);

    blank_space();
    printf("tadaa!\n");

    return 0;
}

///////////////////////////////////////////////////////////////////////

// extract a single word out of the standard input, of not
// more than limit characters, argument array W must be
// limit+1 characters or bigger.
// function adapted from Figure 7.13 of Programming, Problem Solving, 
// and Abstraction with C, by Alistair Moffat

int
getword(char W[], int limit) {
	int c, len=0;
	/* first, skip over any non alphabetics */
	while ((c=getchar())!=EOF && !isalpha(c)) {
		/* do nothing more */
	}
	if (c==EOF) {
		return EOF;
	}
	/* ok, first character of next word has been found */
	W[len++] = c;
	while (len<limit && (c=getchar())!=EOF && isalpha(c)) {
		/* another character to be stored */
		W[len++] = c;
	}
	/* now close off the string */
	W[len] = '\0';
	return 0;
}

///////////////////////////////////////////////////////////////////////
// Helper functions for printing blank lines and spaces

void blank_line(void) {
    printf("    ----\n");
}
void blank_space(void) {
    printf("\n");
}

///////////////////////////////////////////////////////////////////////
// Stage 1 required output:
// prints number of candidates, votes and last row of votes

void stage1(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int *num_candidates,
            int *num_votes) {
    int i,j;
    if (scanf("%d", num_candidates) != 1) {
        exit(EXIT_FAILURE);
    }

    for (i=0; i < *num_candidates; i++) {
        // reads each candidate's name into candidates
        // each name is stored in candidates[i]
        getword(cand[i], MAX_NAME_LENGTH);
        eliminated[i] = 0;
    }

    // highest rank is assigned to rank1 and rest into votes[num_votes][j]
    *num_votes = 0;
    while (*num_votes < MAX_VOTERS) {
        // assume success unless one fails
        int all_ranks_read = 1;

        for (j = 0; j < *num_candidates; j++) {
            // read the rank for this candidate
            if (scanf("%d", &votes[*num_votes][j])!=1) {
                // if a rank cannot be read, stop reading further ballots
                all_ranks_read = 0; 
                break;
            }
            // store a copy of initial votes (used in later stages)
            original_votes[*num_votes][j]=votes[*num_votes][j];
        }
        if (!all_ranks_read) {
            break;
        }
        (*num_votes)++;
    }

    printf("\nStage 1\n=======\n");
    printf("read %d candidates and %d votes\n", *num_candidates, *num_votes);
    if (*num_votes == 0) { 
        printf("no votes were read\n"); 
        return; 
    }
    // print preferences of the last voter
    printf("voter %d preferences...\n", *num_votes);
    int last = *num_votes - 1;
    // for each rank, find which candidate has that rank in the voter's ballot
    for (int rank = 1; rank <= *num_candidates; rank++) {
        for (i = 0; i < *num_candidates; i++) {
            if (votes[last][i]==rank) {
                printf("    rank %2d: %s\n", rank, cand[i]);
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////
//Helper function for stage 2
// Count first-preference votes (active candidates only)

void count_first_pref(int counts[],
                      int votes[][MAX_CANDIDATES], 
                      int eliminated[], 
                      int num_candidates, 
                      int num_votes) {
    int i, v, c;
    // Reset the counts array to 0 for all candidates            
    for (i = 0; i < num_candidates; i++) {
        counts[i] = 0;
    } 
    // Look for the first candidate ranked 1 by the voter
    for (v = 0; v < num_votes; v++) {
        for (c = 0; c < num_candidates; c++) {
            // only count if candidate is active and rank is 1
            if (!eliminated[c] && votes[v][c] == 1) {
                counts[c]++; 
                break;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////
// Find the index of the lowest-vote candidate among active ones

int find_lowest_candidate(int counts[],
                          int eliminated[],
                          int num_candidates) {
    
    int min_index = -1;
    // Find the first active candidate to initialize min_index
    for (int i = 0; i < num_candidates; i++) {
        if (!eliminated[i]) {
            min_index=i;
            break;
        }
    }

    // If no active candidate found, return -1
    if (min_index == -1) {
        return -1;
    }

    // Compare the rest to find the one with the lowest votes
    for (int i = min_index + 1; i < num_candidates; i++) {
        if (!eliminated[i] && counts[i] < counts[min_index]) {
            min_index = i;
        }
    }
    return min_index;
}

///////////////////////////////////////////////////////////////////////
// Redistribute votes after a candidate is eliminated
void redistribute_votes(int elim_index,
                        int votes[][MAX_CANDIDATES],
                        int num_candidates,
                        int num_votes) {
    // adjust ballot after elimination                        
    for (int voter = 0; voter < num_votes; voter++) {
        int eliminated_rank = votes[voter][elim_index];

        // Skip the voter if they did not rank the eliminated candidate
        if (eliminated_rank == 0) {
            continue;
        } 

        // Remove eliminated candidate from voter's ballot
        votes[voter][elim_index] = 0;

        // All candidates ranked lower than eliminated 
        // are movied up by one rank
        // their rank number decreases
        for (int cand = 0; cand < num_candidates; cand++) {
            int current_rank = votes[voter][cand];
            if (current_rank > eliminated_rank) {
                votes[voter][cand] = current_rank - 1;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// Print the results of a round without sorting by votes

void print_unsorted_round(int round, int counts[],
                          char cand[][MAX_NAME_LENGTH],
                          int eliminated[],
                          int num_candidates) {
    int i;
    printf("round %d...\n", round);

    // Calculate total votes for active candidates
    int total = 0;
    for (i = 0;i < num_candidates; i++)
        if (!eliminated[i]) {
            total+=counts[i];
        }

    // Print each active candidate’s votes and percentage
    for (int i = 0; i < num_candidates; i++) {
        if (!eliminated[i]) {
            double pctg; 
            if (total > 0) {
                pctg = (100.0 * counts[i]) / total;
            } else {
                pctg = 0.0;
            }
            // space formatting to align with testcases
            printf("    %-19s : %3d votes, %5.1f%%\n",cand[i],counts[i],pctg);
        }
    }
    blank_line();
}

///////////////////////////////////////////////////////////////////////
// Stage 3 helper functions
// Compare two candidates by their current vote counts

int compare_candidates(int a,int b,int counts[]) {
    int countA = counts[a];
    int countB = counts[b];
    // ensures descending order - higher votes come first
    if (countA > countB) {
        // a should come before b
        return -1; 
    }
    if (countA < countB) {
        // a should come after b
        return 1;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////
// Function adapted from Figure 7.3 page 104 of 
// Programming, Problem Solving, and Abstraction with C, 
// written by Alistair Moffat
// Altered to sort in descending order

void insertion_sort_indices(int indices[], int n, int counts[]) {
    // starting from the second element
    // as the first one is sorted by itself
    for (int i = 1; i < n; i++) {
        int temp = indices[i];
        int j = i - 1;

        // shift all previous indices that should come after temp
        while (j >= 0 && compare_candidates(temp, indices[j], counts) < 0) {
            indices[j + 1] = indices[j];
            j--;
        }
        indices[j + 1] = temp;
    }
}

///////////////////////////////////////////////////////////////////////
// print sorted summary of the current round

void print_sorted_round(int round,int counts[],
                        char cand[][MAX_NAME_LENGTH],
                        int eliminated[],
                        int num_candidates) {
    double pctg;
    int idxs[MAX_CANDIDATES]; 
    int active = 0,total = 0;

    for (int i = 0; i < num_candidates; i++) {
        if (!eliminated[i]) {
            // if candidate is not eliminated
            // store its index and add to total votes
            idxs[active] = i;
            active += 1; 
            total += counts[i];
        }
    }

    insertion_sort_indices(idxs,active,counts);

    printf("round %d...\n", round);
    // print each candidate it sorted order
    for (int k = 0; k < active; k++) {
        // give candidate index from candidates array
        int i = idxs[k];
        if (total > 0) {
            pctg = (100.0 * counts[i]) / total;
        } 
        printf("    %-19s : %3d votes, %5.1f%%\n",cand[i],counts[i],pctg);
        
    }
    blank_line();
}

//////////////////////////////////////////////////////////////////////////////
// Helper function for both stage 2 and 3
// This avoids having repeated code between stages 2 and 3

void run_elimination_round(int sorted,
                           char cand[][MAX_NAME_LENGTH],
                           int votes[][MAX_CANDIDATES],
                           int eliminated[],
                           int num_candidates,
                           int num_votes) {
    int i, round = 1, counts[MAX_CANDIDATES];

    while (1) {
        count_first_pref(counts,votes,eliminated,num_candidates,num_votes);
        if (sorted) {
            print_sorted_round(round,counts,cand,eliminated,num_candidates);
        }
        else {
            print_unsorted_round(round,counts,cand,eliminated,num_candidates);
        }

        int total_active = 0;
        for (i = 0; i < num_candidates; i++)
            if (!eliminated[i]) {
                total_active+=counts[i];
            }

        // check for majority
        for (i = 0; i < num_candidates; i++){
            if (!eliminated[i] && counts[i] > total_active/2.0){
                printf("    %s is declared elected\n", cand[i]);
                return;
            }
        }

        int elim = find_lowest_candidate(counts,eliminated,num_candidates);
        if (elim == -1){
            printf("No candidate can be eliminated (tie or no active votes)\n");
            return;
        }
        printf("    %s is eliminated and votes distributed\n\n",cand[elim]);
        eliminated[elim]=1;
        redistribute_votes(elim,votes,num_candidates,num_votes);
        round++;
    }
}

///////////////////////////////////////////////////////////////////////
// Stage 2 required output: 
// trace elimination to show a winner

void stage2(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int num_candidates,
            int num_votes) {

    printf("\nStage 2\n=======\n");
    for (int i = 0; i < num_candidates; i++) {
        eliminated[i] = 0;  
    }
    // 0 means unsorted
    run_elimination_round(0,cand,votes,eliminated,num_candidates,num_votes);
}

////////////////////////////////////////////////////////////////////////
// Stage 3 required output:
// each round of output in decreasing vote count order

void stage3(char cand[][MAX_NAME_LENGTH],
            int votes[][MAX_CANDIDATES],
            int original_votes[][MAX_CANDIDATES],
            int eliminated[],
            int num_candidates,
            int num_votes) {

    printf("\nStage 3\n=======\n");
    for (int v = 0; v < num_votes; v++)
        for (int c = 0; c < num_candidates; c++)
            votes[v][c]=original_votes[v][c];

    for (int i = 0; i < num_candidates; i++) {
        eliminated[i]=0;
    } 
    // 1 means sorted
    run_elimination_round(1,cand,votes,eliminated,num_candidates,num_votes);
}

////////////////////////////////////////////////////////////////////////

