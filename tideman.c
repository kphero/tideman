#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_cycle(int j, int k);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
        {
            if (strcmp(candidates[i], name) == 0)
            {
                ranks[rank] = i;
                return true;
            }
        }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[j][i] > preferences[i][j])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
        // printf("add_pairs:\n");
        // for(int i = 0; i < pair_count; i++)
        // {
        // printf("%i ", pairs[i].winner);
        // printf("%i ", pairs[i].loser);
        // printf("\n");
        // }
        return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int s[candidate_count];

    for (int i = 0; i < candidate_count; i++)
    {
        int w = preferences[pairs[i].winner][pairs[i].loser];
        int l =  preferences[pairs[i].loser][pairs[i].winner];
        s[i] = w - l;
    }
    // sort function
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (s[i] < s[j])
            {
                int n = s[i];
                int m = s[j];
                s[i] = m;
                s[j] = n;
                int nw = pairs[i].winner;
                int mw = pairs[j].winner;
                pairs[i].winner = mw;
                pairs[j].winner = nw;
                int nl = pairs[i].loser;
                int ml = pairs[j].loser;
                pairs[i].loser = ml;
                pairs[j].loser = nl;
            }
        }
    }
        // printf("sort_pairs:\n");
        //     for(int i = 0; i < pair_count; i++)
        // {
        // printf("%i ", pairs[i].winner);
        // printf("%i ", pairs[i].loser);
        // printf("\n");
        // }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (is_cycle(pairs[i].winner, pairs[i].loser) == false)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
        else
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
    return;
}

//Check cycle
bool is_cycle(int j, int k)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (locked[k][i] == true && i == j)
        {
            return true;
        }
        else if (locked[k][i] == true)
        {
            return is_cycle(j, i);
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = 0; j < pair_count; j++)
        {
            if (locked[i][j] != locked[j][i])
            {
                printf("%s\n", candidates[pairs[i].winner]);
            }
        }
    }
        // for(int i = 0; i < candidate_count; i++)
        // {
        // for(int j = 0; j < candidate_count; j++)
        // printf("%i ", preferences[i][j]);
        // printf("\n");
        // }
        // for(int i = 0; i < pair_count; i++)
        // {
        // for(int j = 0; j < pair_count; j++)
        // printf("%d ", locked[i][j]);
        // printf("\n");
        // }
    return;
}