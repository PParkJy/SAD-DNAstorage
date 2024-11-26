/*
** Copyright 2014 Guillaume Filion, Eduard Valera Zorita and Pol Cusco.
**
** File authors:
**  Guillaume Filion     (guillaume.filion@gmail.com)
**  Eduard Valera Zorita (eduardvalera@gmail.com)
**
** License: 
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*/

#include "trie.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define PAD 5              // Position of padding nodes.
#define EOS -1             // End Of String, for 'dash()'.

// Translation table to insert nodes in the trie.
//          ' ': PAD (5)
//     'a', 'A': 1
//     'c', 'C': 2
//     'g', 'G': 3
//     't', 'T': 4
static const int translate[256] = { 
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 PAD,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,1,0,2,0,0,0,3,0,0,0,0,0,0,0,0,
   0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,
   0,1,0,2,0,0,0,3,0,0,0,0,0,0,0,0,
   0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   
};

// static const int translate[1024] = { 
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//  PAD,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,1,0,2,0,0,0,3,0,0,0,0,0,0,0,0,
//    0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,
//    0,1,0,2,0,0,0,3,0,0,0,0,0,0,0,0,
//    0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
// };



// Translation table to query a sequence in the trie.
// In the table below, non DNA letters are set to a numerical
// value of 6, which will always cause of mismatch with
// sequences translated from the table above. 'PAD' and '-' are
// the only non DNA symbols that match themselves.
//          ' ': PAD (5)
//          '-': 0
//     'a', 'A': 1
//     'c', 'C': 2
//     'g', 'G': 3
//     't', 'T': 4
static const int altranslate[256] = { 
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
 PAD,6,6,6,6,6,6,6,6,6,6,6,6,0,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,1,6,2,6,6,6,3,6,6,6,6,6,6,6,6,
   6,6,6,6,4,6,6,6,6,6,6,6,6,6,6,6,
   6,1,6,2,6,6,6,3,6,6,6,6,6,6,6,6,
   6,6,6,6,4,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
};
// static const int altranslate[1024] = { 
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//  PAD,6,6,6,6,6,6,6,6,6,6,6,6,0,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,1,6,2,6,6,6,3,6,6,6,6,6,6,6,6,
//    6,6,6,6,4,6,6,6,6,6,6,6,6,6,6,6,
//    6,1,6,2,6,6,6,3,6,6,6,6,6,6,6,6,
//    6,6,6,6,4,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
//    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
// };


struct arg_t {
   gstack_t ** hits;
   gstack_t ** pebbles;
   char        tau;
   int         reallen;
   int         length;
   char        maxtau;
   int       * query;
   int         seed_depth;
   int         height;
   int         thres_sub;
   int         thres_del;
   int         thres_ins;
   int         thres_base;
};

void     dash (node_t*, const int*, struct arg_t, struct error_value, const int);
void     destroy_from (node_t*, void(*)(void*), int, int, int);
int      get_height (trie_t*);
void     init_pebbles (node_t*);
node_t * insert (node_t *, int);
node_t * insert_wo_malloc (node_t *, int, node_t *);
node_t * new_trienode (void);
void     poucet (node_t*, int, struct arg_t, int, int **, int);
int      recursive_count_nodes (node_t * node, int, int);

void init_matrix (int, int **);
void show_matrix (int, int **);
error_value find_err (int, int **);

void init_matrix(int length, int ** edit_matrix){
   for(int j = 0; j < length+1 ; j++){
      for (int k = 0; k < length+1 ; k++){
         if (j == 0){
            edit_matrix[j][k] = k;
         }
         else if (k == 0){
            edit_matrix[j][k] = j;
         }
         else{
            edit_matrix[j][k] = -3;
         }
      }
   }
}

void show_matrix(int length, int ** edit_matrix){
   for(int j = 0; j < length+1 ; j++){
      for (int k = 0; k < length+1 ; k++){
         fprintf(stderr, "%d ", edit_matrix[j][k]);
      }
      fprintf(stderr, "\n");
   }
   fprintf(stderr, "\n");
}

error_value find_err(int end_depth, int ** edit_matrix){
   error_value errs;
   int err_subs = 0;
   int err_dels = 0;
   int err_inss = 0;

   int edit_err = edit_matrix[end_depth][end_depth];
   int err_edits = 0;

   int row = end_depth;
   int col = end_depth;
   int del_chk = 0;
   int ins_chk = 0;

   while((err_edits < edit_err)){
      
      if (edit_matrix[row][col] == 0)
         break;

      if (row == 0 && col > 0){
         err_inss++;
         col--;
         continue;
      }

      else if (col == 0 && row > 0){
         err_dels++;
         row--;
         continue;
      }

      if ((edit_matrix[row][col] == edit_matrix[row-1][col-1] + 1) && (edit_matrix[row-1][col-1] + 1 >= 0) && (row-1 >= 0) && (col-1 >=0)){
         err_subs++;
         row--;
         col--;
         del_chk = 0;
         ins_chk = 0;
      }

      else if ((edit_matrix[row][col] == edit_matrix[row-1][col] + 1) && (edit_matrix[row-1][col] + 1 >= 0) && (row-1 >= 0) && (col >=0)){
         err_dels++;
         row--;
         del_chk = 1;
         if (ins_chk == 1){
            err_dels--;
            err_inss--;
            err_subs++;
            ins_chk = 0;
         }
      }

      else if ((edit_matrix[row][col] == edit_matrix[row][col-1] + 1) && (edit_matrix[row][col-1] + 1 >= 0) && (row >= 0) && (col-1 >=0)){
         err_inss++;
         col--;
         ins_chk = 1;
         if (del_chk == 1){
            err_dels--;
            err_inss--;
            err_subs++;
            del_chk = 0;
         }
      }

      else if ((edit_matrix[row][col] == edit_matrix[row-1][col-1]) && (row-1 >= 0) && (col-1 >=0)){
         del_chk = 0;
         ins_chk = 0;
         row--;
         col--;
      }

      err_edits = err_subs + err_dels + err_inss;
   }

   errs.subs = err_subs;
   errs.dels = err_dels;
   errs.inss = err_inss;
   errs.edits = err_subs + err_dels + err_inss;

   //fprintf(stderr, "===================================> edits: %d, %d, %d, %d\n", errs.subs, errs.dels, errs.inss, errs.edits);

   //fprintf(stderr, "%d %d\n", row, col);

   return errs;
}

int real_strlen(const char *str) {
    int count = 0;
    while (*str != '\0') {
        if (*str != ' ') {
            count++;
        }
        str++;
    }
    return count;
}


// Globals.
int ERROR = 0;


gstack_t * const TOWER_TOP;

int get_height(trie_t *trie) { return trie->info->height; }


// ------  SEARCH FUNCTIONS ------ //

int
search
(
         trie_t   *  trie,
   const char     *  query,
   const int         tau,
         gstack_t ** hits,
         int         start_depth,
   const int         seed_depth,
   const int thres_sub,
   const int thres_del,
   const int thres_ins,
   const int thres_base
)
// SYNOPSIS:                                                              
//   Front end query of a trie with the "poucet search" algorithm. Search 
//   does not start from root. Instead, it starts from a given depth      
//   corresponding to the length of the prefix from the previous search.  
//   Since initial computations are identical for queries starting with   
//   the same prefix, the search can restart from there. Each call to     
//   the function starts ahead and seeds pebbles for the next query.      
//                                                                        
// PARAMETERS:                                                            
//   trie: the trie to query                                              
//   query: the query as an ascii string                                  
//   tau: the maximum edit distance                                       
//   hits: a hit stack to push the hits                                   
//   start_depth: the depth to start the search                           
//   seed_depth: how deep to seed pebbles                                 
//                                                                        
// RETURN:                                                                
//   A pointer to 'hits' node array.                                      
//                                                                        
// SIDE EFFECTS:                                                          
//   The non 'const' parameters of the function are modified. The node    
//   array can be resized (which is why the address of the pointers is    
//   passed as a parameter) and the trie is modified by the trailinng of  
//   effect of the search.                                                
{
   ERROR = 0;
   
   int height = get_height(trie);
   if (tau > TAU) {
      fprintf(stderr, "error: requested tau greater than %d\n", TAU);
      return __LINE__;
   }

   //fprintf(stderr, "seq: %s\n", query);
   int length = strlen(query);
   int real_length = real_strlen(query);

   if (length > height) {
      fprintf(stderr, "error: query longer than allowed max\n");
      return __LINE__;
   }

   // Make sure the cache is allocated.
   info_t *info = trie->info;

   start_depth = 0;
   // Reset the pebbles that will be overwritten.
   start_depth = max(start_depth, 0);
   for (int i = start_depth+1 ; i <= min(seed_depth, height) ; i++) {
      info->pebbles[i]->nitems = 0;
   }
   
   // Translate the query string. The first 'char' is kept to store
   // the length of the query, which shifts the array by 1 position.
   int translated[M];
   translated[0] = length;
   translated[length+1] = EOS;
   for (int i = max(0, start_depth-TAU) ; i < length ; i++) {
      translated[i+1] = altranslate[(int) query[i]];
   }
   
   // Set the search options.
   struct arg_t arg = {
      .hits    = hits,
      .query   = translated,
      .reallen = real_length,
      .length = length,
      .tau     = tau,
      .pebbles = info->pebbles,
      .seed_depth    = seed_depth,
      .height  = height,
      .thres_sub = thres_sub,
      .thres_del = thres_del,
      .thres_ins = thres_ins,
      .thres_base = thres_base
   };

   int **edit_matrix;
   int trie_pad = 0;
   edit_matrix = (int **)calloc(length+1, sizeof(int *));
   for(int j = 0; j < length+1; j++){
      edit_matrix[j] = (int *)calloc(length+1, sizeof(int));
   }

   init_matrix(length, edit_matrix);

   // Run recursive search from cached nodes.
   gstack_t *pebbles = info->pebbles[start_depth];
   for (unsigned int i = 0 ; i < pebbles->nitems ; i++) {
      node_t *start_node = (node_t *) pebbles->items[i];
      //fprintf(stderr, "-----------chk-------\n");
      poucet(start_node, start_depth + 1, arg, length, edit_matrix, trie_pad);
   }

         //print edit distance matrix
   for (int j = 0 ; j < length+1; j++){
      free(edit_matrix[j]);
   }
   free(edit_matrix);

   // Return the error code of the process (the line of
   // the last error) and 0 if everything went OK.
   return check_trie_error_and_reset();
}

void
poucet
(
   node_t * restrict node,
   const  int      depth,
   struct arg_t    arg,
   int length,
   int ** edit_matrix,
   int trie_pad
)
// SYNOPSIS:                                                              
//   Back end recursive "poucet search" algorithm. Most of the time is    
//   spent in this function. The focus node sets the values of  an L-     
//   shaped section (but with the angle on the right side) of the dynamic 
//   programming table for its children. One of the arms of the L is      
//   identical for all the children and is calculated separately. The     
//   rest is classical dynamic programming computed in the 'cache' struct 
//   member of the children. The path of the last 8 nodes leading to the  
//   focus node is encoded by a 32 bit integer, which allows to perform   
//   dynamic programming without parent pointer.                          
//
//   All the leaves of the trie are at a the same depth called the   
//   "height", which is the depth at which the recursion is stopped to    
//   check for hits. If the maximum edit distance 'tau' is exceeded the   
//   search is interrupted. On the other hand, if the search has passed   
//   trailing depth and 'tau' is exactly reached, the search finishes by  
//   a 'dash()' which checks whether an exact suffix can be found.        
//   While trailing, the nodes are pushed in the g_stack 'pebbles' so     
//   they can serve as starting points for future searches.               
//
//   Since not all the sequences have the same length, they are prefixed
//   with the 'PAD' character (value 5, printed as white space) so that
//   the total length is equal to the height of the trie. This imposes
//   an important modification to the recursion, indicated by the label
//   "PAD exception" on two different lines of the code below. Without
//   this modification, "AAAAATA" and "AAAAA" would be aligned this way.
//
//                               AAAAATA
//                               x||||x|
//                               _AAAAAA
//
//   However, the best alignment is the following.
//
//                               AAAAATA
//                               |||||x|
//                               AAAAA_A
//
//   The solution is to initialize the alignment score to 0 whenever the
//   padding character is met, which in effect is equivalent to ignoring
//   starting the alignment after the PADs.
//                                                                        
// PARAMETERS:                                                            
//   node: the focus node in the trie                                     
//   depth: the depth of the children in the trie.                        
//                                                                        
// RETURN:                                                                
//   'void'.                                                              
//                                                                        
// SIDE EFFECTS:                                                          
//   Same as 'search()', it modifies nodes of the trie and the node       
//   array 'arg.hits' where hits are pushed.                              
{

   // This makes it easier to distinguish the part that goes upward,
   // with positive index and requiring the path, from the part that
   // goes horizontally, with negative index and requiring previous
   // characters of the query.

   char *pcache = node->cache + TAU;
   // Risk of overflow at depth lower than 'tau'.
   int maxa = min((depth-1), arg.tau);

   // Penalty for match/mismatch and insertion/deletion resepectively.
   unsigned char mmatch;
   unsigned char shift;

   // Part of the cache that is shared between all the children.
   //char common[9] = {1,2,3,4,5,6,7,8,9}; --jy fix
   //char common[11] = {1,2,3,4,5,6,7,8,9,10,11};
   char common[31] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

   // The branch of the L that is identical among all children
   // is computed separately. It will be copied later.
   int32_t path = node->path;
   
   // Upper arm of the L (need the path).
   if (maxa > 0) {
      // Special initialization for first character. If the previous
      // character was a PAD, there is no cost to start the alignment.
      // This is the "PAD exeption" mentioned in the SYNOPSIS.
      mmatch = (arg.query[depth-1] == PAD ? 0 : pcache[maxa]) +
                  ((path >> 4*(maxa-1) & 15) != arg.query[depth]);
      shift = min(pcache[maxa-1], common[maxa]) + 1;
      common[maxa-1] = min(mmatch, shift);

      for (int a = maxa-1 ; a > 0 ; a--) {
         mmatch = pcache[a] + ((path >> 4*(a-1) & 15) != arg.query[depth]);
         shift = min(pcache[a-1], common[a]) + 1;
         common[a-1] = min(mmatch, shift);

      }
   }

   node_t *child;

   int i = 0;   
   for (i = 0 ; i < 6 ; i++) {

      // Skip if current node has no child at this position.
      if ((child = node->child[i]) == NULL) {
         continue;
      }
      
      if(i == 5){
         trie_pad++;
      }
      // Same remark as for parent cache.
      //char local_cache[] = {9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9}; --jy fix
      //char local_cache[] = {11,10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10,11};
      char local_cache[] = {31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

      char *ccache = depth == arg.height ? local_cache + 31 : child->cache + TAU; 
      //local_cache + 11 : child->cache + TAU; 
      //local_cache + 9 : child->cache + TAU; -- jy fix

      memcpy(ccache+1, common, TAU * sizeof(char));
      // Horizontal arm of the L (need previous characters).
      

      if (maxa > 0) {
         // See comment above for initialization.
         // This is the "PAD exeption" mentioned in the SYNOPSIS.
         mmatch = ((path & 15) == PAD ? 0 : pcache[-maxa]) +
                     (i != arg.query[depth-maxa]);
         shift = min(pcache[1-maxa], maxa+1) + 1;
         ccache[-maxa] = min(mmatch, shift);
         
         for (int a = maxa-1 ; a > 0 ; a--) {
            mmatch = pcache[-a] + (i != arg.query[depth-a]);
            shift = min(pcache[1-a], ccache[-a-1]) + 1;
            ccache[-a] = min(mmatch, shift);

         }
      }

      // Center cell (need both arms to be computed).
      mmatch = pcache[0] + (i != arg.query[depth]);
      shift = min(ccache[-1], ccache[1]) + 1;
      ccache[0] = min(mmatch, shift);

      //fprintf(stderr, "dep: %d, maxa: %d\n", depth, maxa);
      //fprintf(stderr, "%d, %d\n", i, arg.query[depth]);
   
      edit_matrix[depth][depth] = ccache[0];

      for (int a = maxa ; a > 0 ; a--){
         edit_matrix[depth-a][depth] = common[a-1];
         edit_matrix[depth][depth-a] = ccache[-a];   
      }

      edit_matrix[depth-1][depth-1] = pcache[0];   

      if (i == 5){
      //    for (int b = 1; b < length + 1; b++)
      //       edit_matrix[depth][b] = -5;
         edit_matrix[depth][0] = 0;
      }

      if (arg.query[depth] == 5){
      //    for (int b = 1; b < length + 1; b++)
      //       edit_matrix[b][depth] = -5;
         edit_matrix[0][depth] = 0;
      }
      

      edit_matrix[0][0] = 0;


      // Stop searching if 'tau' is exceeded.
      if (ccache[0] > arg.tau) {
         //fprintf(stderr, "------------1------------\n");
         //show_matrix(length, edit_matrix);
         
         continue;
      }

      // Reached height of the trie: it's a hit!
      if (depth == arg.height) {   
         
         int end_depth = depth;

         error_value errs;
         errs = find_err(end_depth, edit_matrix);

         //fprintf(stderr, " type: 1 ==== %d %d %d %d %d %d %d\n", arg.length, arg.reallen, arg.height - trie_pad, errs.subs, errs.dels, errs.inss, errs.edits);
 
         if (arg.reallen == arg.height - trie_pad){
            // same length
            if (errs.edits <= arg.thres_base){
               if (push(child, arg.hits + ccache[0])) ERROR = __LINE__;
            }
         }
         else {
            //
            int len_diff = arg.reallen - (arg.height - trie_pad);
            if (len_diff < 0){
               len_diff *= -1;
            }

            //fprintf(stderr, "diff: %d\n", len_diff);

            if (errs.edits - len_diff <= arg.thres_base){
               if (push(child, arg.hits + ccache[0])) ERROR = __LINE__;
            }
         }

         continue;
      }

      // Cache nodes in pebbles when trailing.
      if (depth <= arg.seed_depth) {
         //fprintf(stderr, "------------3------------\n");
         if (push(child, (arg.pebbles)+depth)) ERROR = __LINE__;
      }

      if (depth > arg.seed_depth) {
         //fprintf(stderr, "------------4------------\n");
         // Use 'dash()' if no more mismatches allowed.
         int can_dash = 1;
         for (int a = -maxa ; a < maxa+1 ; a++) {
            if (ccache[a] < arg.tau) {     
               can_dash = 0;
               break;
            }
         }

         if (can_dash) {
            int end_depth = depth;
            error_value errs;
            errs = find_err(end_depth, edit_matrix);
            dash(child, arg.query+depth+1, arg, errs, trie_pad);
            
            continue;
         }
      }

      poucet(child, depth+1, arg, length, edit_matrix, trie_pad);
      

   }
}



void
dash
(
          node_t * restrict node,
   const  int    * restrict suffix,
   struct arg_t    arg,
   struct error_value errs,
   const int trie_pad
)
// SYNOPSIS:                                                              
//   Checks whether node has the given suffix and reports a hit if this   
//   is the case.                                                         
//                                                                        
// PARAMETERS:                                                            
//   node: the node to test                                               
//   suffix: the suffix to test as a translated sequence                  
//                                                                        
// RETURN:                                                                
//   'void'.                                                              
//                                                                        
// SIDE EFFECTS:                                                          
//   Updates 'arg.hits' if the suffix is found.                           
{

   int c;
   node_t *child;
   //fprintf(stderr, "end:  %d\n", end_depth);
   // Early return if the suffix path is broken.
   while ((c = *suffix++) != EOS) {
      if ((c > 4) || (child = (node_t *) node->child[c]) == NULL) {
         return;
      }
      node = child;
   }
   
    //fprintf(stderr, " type: 2 ==== %d %d %d %d %d %d %d\n", arg.length, arg.reallen, arg.height-trie_pad, errs.subs, errs.dels, errs.inss, errs.edits);
 
   if (arg.reallen == arg.height - trie_pad){
      // same length
      if (errs.edits <= arg.thres_base){
         if (push(node, arg.hits + arg.tau)) ERROR = __LINE__;
      }
   }
   else if (arg.reallen < arg.height - trie_pad){
      // short than max length
      if (errs.edits - (arg.height - arg.reallen) <= arg.thres_base){
         if (push(node, arg.hits + arg.tau)) ERROR = __LINE__;
      }
   }
   
   return;

}


// ------  TRIE CONSTRUCTION AND DESTRUCTION  ------ //


trie_t *
new_trie
(
   unsigned int  height
)
// SYNOPSIS:                                                              
//   Front end trie constructor.                                          
//                                                                        
// PARAMETERS:                                                            
//   height: the fixed depth of the leaves                                
//                                                                        
// RETURN:                                                                
//   A pointer to trie root with meta information and no children.        
{

   if (height < 1) {
      fprintf(stderr, "error: the minimum trie height is 1\n");
      ERROR = __LINE__;
      return NULL;
   }

   trie_t *trie = malloc(sizeof(trie_t));
   if (trie == NULL) {
      fprintf(stderr, "error: could not create trie\n");
      ERROR = __LINE__;
      return NULL;
   }

   node_t *root = new_trienode();
   
   if (root == NULL) {
      fprintf(stderr, "error: could not create root\n");
      ERROR = __LINE__;
      free(trie);
      return NULL;
   }

   info_t *info = malloc(sizeof(info_t));
   if (info == NULL) {
      fprintf(stderr, "error: could not create trie\n");
      ERROR = __LINE__;
      free(root);
      free(trie);
      return NULL;
   }

   // Set the values of the meta information.
   info->height = height;
   info->pebbles = new_tower(M);

   // Push the root to the ground level of 'pebbles'.
   // This will be the only node at this level for
   // the lifetime of the trie.
   if (info->pebbles == NULL || push(root, info->pebbles)) {
      fprintf(stderr, "error: could not create trie\n");
      ERROR = __LINE__;
      free(info);
      free(root);
      free(trie);
      return NULL;
   }

   trie->root = root;
   trie->info = info;

   return trie;

}


node_t *
new_trienode
(void)
// SYNOPSIS:                                                              
//   Back end constructor for a trie node. All values are initialized to  
//   null, except the cache for dynamic programming which is initialized  
//   as a root node.                                                      
//                                                                        
// RETURN:                                                                
//   A pointer to trie node with no data and no children.                 
{

   node_t *node = calloc(1, sizeof(node_t));
   if (node == NULL) {
      fprintf(stderr, "error: could not create trie node\n");
      ERROR = __LINE__;
      return NULL;
   }

   // Initialize the cache. This is important for the
   // dynamic programming algorithm.
   //const char init[] = {8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8}; --jy fix
   //const char init[] = {10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10};
   const char init[] = {30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
   memcpy(node->cache, init, 2*TAU+1);
   return node;

}


void **
insert_string
(
         trie_t * trie,
   const char   * string
)
// SYNOPSIS:                                                              
//   Front end function to fill in a trie. Insert a string from root, or  
//   simply return the node at the end of the string path if it already   
//   exists.                                                              
//                                                                        
// RETURN:                                                                
//   The leaf node in case of succes, 'NULL' otherwise.                   
//                                                                        
// NB: This function is not used by 'starcode()'.
{

   int i;

   int nchar = strlen(string);
   if (nchar != get_height(trie)) {
      fprintf(stderr, "error: can only insert string of length %d\n",
            get_height(trie));
      ERROR = __LINE__;
      return NULL;
   }
   
   // Find existing path.
   node_t *node = trie->root;
   for (i = 0 ; i < nchar-1; i++) {
      node_t *child;
      int c = translate[(int) string[i]];
      if ((child = (node_t *) node->child[c]) == NULL) {
         break;
      }
      node = child;
   }

   // Append more nodes.
   for ( ; i < nchar-1 ; i++) {
      int c = translate[(int) string[i]];
      node = insert(node, c);
      if (node == NULL) {
         fprintf(stderr, "error: could not insert string\n");
         ERROR = __LINE__;
         return NULL;
      }
   }

   return node->child + translate[(int) string[nchar-1]];

}


node_t *
insert
(
   node_t * parent,
   int      position
)
// SYNOPSIS:                                                              
//   Back end function to construct tries. Append a child to an existing  
//   node at specifieid position.                                         
//   NO CHECKING IS PERFORMED to make sure that this does not overwrite   
//   an existings node child (causing a memory leak) or that 'c' is an    
//   integer less than 5. Since 'insert' is called exclusiverly by        
//   'insert_string' after a call to 'find_path', this checking is not    
//   required. If 'insert' is called in another context, this check has   
//   to be performed.                                                     
//                                                                        
// PARAMETERS:                                                            
//   parent: the parent to append the node to                             
//   position: the position of the child                                  
//                                                                        
// RETURN:                                                                
//   The appended child node in case of success, 'NULL' otherwise.        
//                                                                        
// NB: This function is not used by 'starcode()'.
{
   // Initilalize child node.
   node_t *child = new_trienode();
   if (child == NULL) {
      fprintf(stderr, "error: could not insert node\n");
      ERROR = __LINE__;
      return NULL;
   }
   // Update child path and parent pointer.
   child->path = (parent->path << 4) + position;
   // Update parent node.
   parent->child[position] = child;

   return child;

}


void **
insert_string_wo_malloc
(
         trie_t  * trie,
   const char    * string,
         node_t ** from_addr
)
// TODO: document the function, explain that 'from_addr' is
// incremented.
// SYNOPSIS:                                                              
//                                                                        
// RETURN:                                                                
{

   int i;

   int nchar = strlen(string);
   if (nchar != get_height(trie)) {
      fprintf(stderr, "error: can only insert string of length %d\n",
            get_height(trie));
      ERROR = __LINE__;
      return NULL;
   }
   
   // Find existing path.
   node_t *node = trie->root;
   for (i = 0 ; i < nchar-1; i++) {
      node_t *child;
      int c = translate[(int) string[i]];
      if ((child = (node_t *) node->child[c]) == NULL) {
         break;
      }
      node = child;
   }

   // Append more nodes.
   for ( ; i < nchar-1 ; i++) {
      int c = translate[(int) string[i]];
      node = insert_wo_malloc(node, c, *from_addr);
      (*from_addr)++; 
   }

   return node->child + translate[(int) string[nchar-1]];

}

node_t *
insert_wo_malloc
(
   node_t * parent,
   int      position,
   node_t * at_address
)
// SYNOPSIS:                                                              
//   Adds a child to the specified node without calling 'malloc()'.
//   The data is copied at the addres given by the 'at_address'
//   pointer.
//   Checks whether the position to insert the node is free. If not,
//   nothing is done and 'NULL' is returned.
//                                                                        
// PARAMETERS:                                                            
//   parent: the parent node on which to add the child.
//   position: a number from 0 to 5 indicating the rank of the child.
//   at_address: the position in memory where to copy the child data.
//                                                                        
// RETURN:                                                                
//   The address of the child node if the insert position is free,
//   'NULL' otherwise.
//
// SIDE EFFECTS:
//   Modifies the data pointed to by 'at_address'. Also modifies the
//   parent node
{

   if (parent->child[position] != NULL) return NULL;
   node_t *newnode = at_address; // for clarity

   // Initialize child data.
   memset(newnode->child, 0, 6 * sizeof(void *));
   newnode->path = (parent->path << 4) + position;
   //const char init[] = {8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8}; -- jy fix
   //const char init[] = {10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10};
   const char init[] = {30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
   memcpy(newnode->cache, init, 2*TAU+1);

   parent->child[position] = newnode;

   return newnode;

}


void
destroy_trie
(
   trie_t * trie,
   int      free_nodes,
   void     (*destruct)(void *)
)
// SYNOPSIS:                                                              
//   Front end function to recycle the memory allocated to a trie. Node   
//   data may or may not be recycled as well, depending on the destructor 
//   passed as argument. This function is essentially a wrapper for       
//   'destroy_nodes_()', the only difference is that it      
//   destroys the meta-data of the trie first.                            
//                                                                        
// PARAMETERS:                                                            
//   trie: the trie to destroy                                            
//   destruct: a function with the same prototype as 'free()'             
//                                                                        
// RETURN:                                                                
//   'void'.                                                              
//                                                                        
// SIDE EFFECTS:                                                          
//   Frees the memory allocated to the nodes of a trie, the meta-data     
//   associated to the root, and possibly the data associated to the tail 
//   nodes.                                                               
{
   // Free the milesones.
   destroy_tower(trie->info->pebbles);
   destroy_from(trie->root, destruct, free_nodes, get_height(trie), 0);
   if (!free_nodes) {
      free(trie->root);
      trie->root = NULL;
   }
   free(trie->info);
   free(trie);
}


void
destroy_from
(
   node_t * node,
   void     (*destruct)(void *),
   int      free_nodes,
   int      maxdepth,
   int      depth
)
// SYNOPSIS:                                                              
//   Back end function to free the memory allocated on a trie. It should  
//   not be called directly on a trie to prevent memory leaks (otherwise  
//   the pebbles will not be freed).                                      
//                                                                        
// RETURN:                                                                
//   'void'.                                                              
//                                                                        
// SIDE EFFECTS:                                                          
//   Frees the memory allocated to the nodes of a trie, and possibly the  
//   data associated to the tail nodes.                                   
{  
   if (node != NULL) {
      if (depth == maxdepth) {
         if (destruct != NULL) (*destruct)(node);
         return;
      }
      for (int i = 0 ; i < 6 ; i++) {
         node_t * child = (node_t *) node->child[i];
         destroy_from(child, destruct, free_nodes, maxdepth, depth+1);
      }
      if (free_nodes) {
         free(node);
         node = NULL;
      }
   }
   return;
}



// ------  UTILITY FUNCTIONS ------ //


gstack_t *
new_gstack
(void)
{
   // Allocate memory for a node array, with 'GSTACK_INIT_SIZE' slots.
   size_t base_size = sizeof(gstack_t);
   size_t extra_size = GSTACK_INIT_SIZE * sizeof(void *);
   gstack_t *new = malloc(base_size + extra_size);
   if (new == NULL) {
      fprintf(stderr, "error: could not create gstack\n");
      ERROR = __LINE__;
      return NULL;
   }
   new->nitems = 0;
   new->nslots = GSTACK_INIT_SIZE;
   return new;
}

gstack_t **
new_tower
(
   int height
)
{
   gstack_t **new = malloc((height+1) * sizeof(gstack_t *));
   if (new == NULL) {
      fprintf(stderr, "error: could not create tower\n");
      ERROR = __LINE__;
      return NULL;
   }
   for (int i = 0 ; i < height ; i++) {
      new[i] = new_gstack();
      if (new[i] == NULL) {
         ERROR = __LINE__;
         fprintf(stderr, "error: could not initialize tower\n");
         while (--i >= 0) {
            free(new[i]);
            new[i] = NULL;
         }
         free(new);
         return NULL;
      }
   }
   new[height] = TOWER_TOP;
   return new;
}


void
destroy_tower
(
   gstack_t **tower
)
{
   for (int i = 0 ; tower[i] != TOWER_TOP ; i++) free(tower[i]);
   free(tower);
}


int
push
(
   void *item,
   gstack_t **stack_addr
)
// SYNOPSIS:
//   Push the item referenced by the pointer to the stack. In case
//   of failure to push the item (because of 'realloc()' failure)
//   the stack is locked and cannot reiceive more items. Locked
//   stack is an indication that something went wrong with it and
//   that at least one item is missing.
//
// ARGUMENTS:
//   item: a void pointer to the item to push
//   stack_addr: the address of the stack pointer (not the pointer).
//
// RETURN:
//   0 upon success, 1 upon failure.
//
// SIDE EFFECT:
//   Modifies the stack pointed to by '*stack_addr', and potentially
//   also the address pointed to by '**stack_addr' if resizing is
//   required.
{

   // Convenience variable for readability.
   gstack_t *stack = *stack_addr;

   // Resize stack if needed.
   if (stack->nitems >= stack->nslots) {
      // If the stack has more items than slots, it is in a
      // locked state and will not receive more items. This
      // allows to 
      if (stack->nitems > stack->nslots) return 1;

      // The stack is not locked, allocate more memory.
      size_t new_nslots = 2 * stack->nslots;
      size_t base_size = sizeof(gstack_t);
      size_t extra_size = new_nslots * sizeof(void *);
      gstack_t *ptr = realloc(stack, base_size + extra_size);
      if (ptr == NULL) {
         // Failed to add item to the stack. Increase
         // 'nitems' beyond 'nslots' to lock the stack.
         stack->nitems++;
         ERROR = __LINE__;
         return 1;
      }
      *stack_addr = stack = ptr;
      stack->nslots = new_nslots;
   }

   // Push item and increase 'nitems'.
   stack->items[stack->nitems++] = item;
   return 0;

}


// Snippet to check whether everything went fine.
// If not, ERROR is the line raising the error.
int check_trie_error_and_reset(void) {
   if (ERROR) {
      int last_error_at_line = ERROR;
      ERROR = 0;
      return last_error_at_line;
   }
   return 0;
}

// Snippet to count nodes of a trie.
int count_nodes(trie_t * trie) {
   return recursive_count_nodes(trie->root, get_height(trie), 0);
}

int recursive_count_nodes(node_t * node, int maxdepth, int depth) {
   int count = 1;
   for (int i = 0 ; i < 6 ; i++) {
      if (node->child[i] == NULL) continue;
      node_t *child = (node_t *) node->child[i];
      count += depth < maxdepth - 1 ?
         recursive_count_nodes(child, maxdepth, depth+1) : 1;
   }
   return count;
}
