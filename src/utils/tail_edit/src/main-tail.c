/*
===========================================================================
Revised by Jiyeon Park for Tailored edit-distance based clustering in 2024.
===========================================================================

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

#include <execinfo.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tail.h"

#define ERRM "starcode error:"

// Prototypes for utilities of the main.
char * outname (char *);
void   say_usage (void);
void   say_version (void);
void   SIGSEGV_handler (int);

char *USAGE =
"\n"
"Usage:"
"  Tailored edit distance-based clustering [options]\n"
"\n"
"  general options:\n"
"    -x --thres-sub: threshold of number of substitutions\n"
"    -y --thres-del: threshold of number of deletions\n"
"    -z --thres-ins: threshold of number of insertions\n"
"    -d --dist: maximum Levenshtein distance (default auto)\n"
"    -t --threads: number of concurrent threads (default 1)\n"
"    -b --bias: the maximum length difference\n"
"    -q --quiet: quiet output (default verbose)\n"
"    -v --version: display version and exit\n"
"\n"
"    If the thresholds (thres-sub, del, ins) are equal to 0, the clustering is conducted by only maximum Levenshtein distance (-d, --dist)\n"
"    If you do not want to adjust the thresholds (thres-sub, del, ins), set these values to 0, and assign a value only to maximum Levenshtein distance(-d, --dist))\n"
"    If you want to adjust the thresholds (thres-sub, del, ins), values should be assigned to these values, while maximum Levenshtein distance (-d, --dist) should be set to 0.\n"
"\n"
"  cluster options:\n"
"    -j --tailored: tailored clustering based on the sphere clustering of starcode\n"
"    -l --length: original length of reads\n"
"\n"
"  input/output options (single file, default)\n"
"    -i --input: input file (default stdin)\n"
"    -o --output: output file (default stdout)\n"
"\n"
"  input options (paired-end fastq files)\n"
"    -1 --input1: input file 1\n"
"    -2 --input2: input file 2\n"
"\n"
"  output options (paired-end fastq files, --non-redundant only)\n"
"       --output1: output file1 (default input1-starcode.fastq)\n"
"       --output2: output file2 (default input2-starcode.fastq)\n"
"\n"
"  output format options\n"
"       --non-redundant: remove redundant sequences from input file(s)\n"
"       --print-clusters: outputs cluster compositions\n"
"       --seq-id: print sequence id numbers (1-based)\n"
"       --tidy: print each sequence and its centroid\n"
"\n";

void say_usage(void) { fprintf(stderr, "%s\n", USAGE); }
void say_version(void) { fprintf(stderr, VERSION "\n"); }

void SIGSEGV_handler(int sig) {
   void *array[10];
   size_t size;

   // get void*'s for all entries on the stack
   size = backtrace(array, 10);

   // print out all the frames to stderr
   fprintf(stderr, "Error: signal %d:\n", sig);
   backtrace_symbols_fd(array, size, STDERR_FILENO);
   exit(1);
}


char *
outname
(
   char *path
)
{

   char * name = calloc(320,1);
   if (strlen(path) > 310) {
      fprintf(stderr, "input file name too long (%s)\n", path);
      abort();
   }

   // Find final dot, append "-starcode" just before.
   // If no final dot, just append starcode as suffix.
   char *c = strrchr(path, '.');
   if (c == NULL) {
      sprintf(name, "%s-starcode", path);
   }
   else {
      *c = '\0';
      sprintf(name, "%s-starcode.%s", path, c+1);
      *c = '.';
   }

   return (char *) name;

}


int
main(
   int argc,
   char **argv
)
{
   // Backtrace handler
   signal(SIGSEGV, SIGSEGV_handler); 

   // Set flags to defaults.
   static int nr_flag = 0;
   static int td_flag = 0;
   static int vb_flag = 1;
   static int cl_flag = 0;
   static int id_flag = 0;
   static int jp_flag = 0;

   // Unset flags (value -1).
   int dist = -1;
   int final_dist = -1;
   int threads = -1;
   int tail_flag = -1;
   double cluster_ratio = -1;

   int threshold_sub = 0;
   int threshold_del = 0;
   int threshold_ins = 0;
   int org_len = 0;
   int bias = 0;

   // Unset options (value 'UNSET').
   char * const UNSET = "unset";
   char * input   = UNSET;
   char * input1  = UNSET;
   char * input2  = UNSET;
   char * output  = UNSET;
   char * output1 = UNSET;
   char * output2 = UNSET;

   if (argc == 1 && isatty(0)) {
      say_usage();
      return EXIT_SUCCESS;
   }

   int c;
   while (1) {
      int option_index = 0;
      static struct option long_options[] = {
         {"print-clusters",    no_argument,       &cl_flag,  1 },
         {"seq-id",            no_argument,       &id_flag,  1 },
         {"non-redundant",     no_argument,       &nr_flag,  1 },
         {"tidy",              no_argument,       &td_flag,  1 },
         {"quiet",             no_argument,       &vb_flag,  0 },
         {"tailored",          no_argument,       &jp_flag, 'j'},
         {"version",           no_argument,              0, 'v'},
         {"dist",              required_argument,        0, 'd'},
         {"cluster-ratio",     required_argument,        0, 'r'},
         {"help",              no_argument,              0, 'h'},
         {"input",             required_argument,        0, 'i'},
         {"input1",            required_argument,        0, '1'},
         {"input2",            required_argument,        0, '2'},
         {"output",            required_argument,        0, 'o'},
         {"threads",           required_argument,        0, 't'},
         {"output1",           required_argument,        0, '3'},
         {"output2",           required_argument,        0, '4'},
         {"thres-sub",         required_argument,        0, 'x'},
         {"thres-del",         required_argument,        0, 'y'},
         {"thres-ins",         required_argument,        0, 'z'},
         {"length",            required_argument,        0, 'l'},
         {"bias",              required_argument,        0, 'k'},
         {0, 0, 0, 0}
      };

      c = getopt_long(argc, argv, "1:2:3:4:x:y:z:k:d:l:hi:o:qjt:r:v",
            long_options, &option_index);
 
      // Done parsing //
      if (c == -1) break;

      switch (c) {
      case 0:
         // A flag was set. //
         break;

      case '1':
         if (input1 == UNSET) {
            input1 = optarg;
         }
         else {
            fprintf(stderr, "%s --input1 set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case '2':
         if (input2 == UNSET) {
            input2 = optarg;
         }
         else {
            fprintf(stderr, "%s --input2 set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case '3':
         if (output1 == UNSET) {
            output1 = optarg;
         }
         else {
            fprintf(stderr, "%s --output1 set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case '4':
         if (output2 == UNSET) {
            output2 = optarg;
         }
         else {
            fprintf(stderr, "%s --output2 set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case 'x':
         threshold_sub = atoi(optarg);
         break;

      case 'y':
         threshold_del = atoi(optarg);
         break;
      
      case 'z':
         threshold_ins = atoi(optarg); 
         break;

      case 'k':
         bias = atoi(optarg);
         break;

      case 'd':
         dist = atoi(optarg);
         if (dist > 0) {            
            if (dist > STARCODE_MAX_TAU) {
               fprintf(stderr, "%s --dist cannot exceed %d\n",
                     ERRM, STARCODE_MAX_TAU);
               return EXIT_FAILURE;
            }

            int dist_chk = threshold_sub + threshold_del + threshold_ins;
            if( dist == 0 && dist_chk == 0){
               fprintf(stderr, "maximum Levenshtein distance and all thresholds are 0. You must set a value for at least one of them.\n");
               say_usage();
               return EXIT_FAILURE;
            }
            else if (dist == dist_chk){
               // USE thres-sub, thres-del, thres-ins
               final_dist = threshold_sub + threshold_del + threshold_ins + bias;
               tail_flag = 0;
               if (final_dist > STARCODE_MAX_TAU) {
                  fprintf(stderr, "%s --dist cannot exceed %d\n",
                        ERRM, STARCODE_MAX_TAU - bias);
                  return EXIT_FAILURE;
               }
               fprintf(stderr, "Threshold_sub: %d, Threshold_del: %d, Threshold_ins: %d\n\n", threshold_sub, threshold_del, threshold_ins);
            }
            else if (dist_chk == 0){
               tail_flag = 1;
               // USE only maximum Levenshtein distance   
               final_dist = dist + bias;
               fprintf(stderr, "Maximum levenshtein distance: %d\n\n", dist);
            }
         }
         else {
            fprintf(stderr, "%s --distance set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;
      
      case 'l':
         org_len = atoi(optarg);
         break;
      
      case 'h':
         // User asked for help. //
         say_version();
         say_usage();
         return 0;

      case 'i':
         if (input == UNSET) {
            input = optarg;
         }
         else {
            fprintf(stderr, "%s --input set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case 'o':
         if (output == UNSET) {
            output = optarg;
         }
         else {
            fprintf(stderr, "%s --output set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case 'q':
         vb_flag = 0;
         break;

      case 'j':
         jp_flag = 1;
         break;

      case 't':
         if (threads < 0) {
            threads = atoi(optarg);
            if (threads < 1) {
               fprintf(stderr,
                     "%s --threads must be a positive " "integer\n", ERRM);
               say_usage();
               return EXIT_FAILURE;
            }
         }
         else {
            fprintf(stderr, "%s --thread set more than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case 'r':
         if (cluster_ratio < 0) {
            cluster_ratio = atof(optarg);
            if (cluster_ratio < 1) {
               fprintf(stderr, "%s --cluster-ratio must be "
                     "greater or equal than 1.0.\n", ERRM);
               say_usage();
               return EXIT_FAILURE;
            }
         }
         else {
            fprintf(stderr,
                  "%s --cluster-ratio set more " "than once\n", ERRM);
            say_usage();
            return EXIT_FAILURE;
         }
         break;

      case 'v':
         say_version();
         return EXIT_SUCCESS;

      default:
         // Cannot parse. //
         say_usage();
         return EXIT_FAILURE;

      }

   }

   if (optind < argc) {
      // If no input is specified, assume first positional argument
      // is the name of the input file.
      if ((optind == argc-1) && (input == UNSET && input1 == UNSET)) {
         input = argv[optind];
      }
      else {
         fprintf(stderr, "%s too many options\n", ERRM);
         say_usage();
         return EXIT_FAILURE;
      }
   }

   // Check options compatibility. //
   if (nr_flag && (cl_flag || id_flag)) {
      fprintf(stderr,
            "%s --non-redundant flag is incompatible with "
            "--print-clusters and --seq-id\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }
   if (input != UNSET && (input1 != UNSET || input2 != UNSET)) {
      fprintf(stderr,
            "%s --input and --input1/2 are incompatible\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }
   if (input1 == UNSET && input2 != UNSET) {
      fprintf(stderr, "%s --input2 set without --input1\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }
   if (input2 == UNSET && input1 != UNSET) {
      fprintf(stderr, "%s --input1 set without --input2\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }
   if (nr_flag && output != UNSET &&
         (input1 != UNSET || input2 != UNSET)) {
      fprintf(stderr, "%s cannot specify --output for paired-end "
            "fastq file with --non-redundant\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }
   if (td_flag && (nr_flag || cl_flag || id_flag)) {
      fprintf(stderr,
            "%s --tidy flag is not compatible with options "
            "--print-clusters, --seq-id and --non-redundant\n", ERRM);
      say_usage();
      return EXIT_FAILURE;
   }

   // Set output type. //
   int output_type;
   if      (nr_flag) output_type = NRED_OUTPUT;
   else if (td_flag) output_type = TIDY_OUTPUT;
   else              output_type = DEFAULT_OUTPUT;

   int cluster_alg;
   cluster_alg = TAILORED_CLUSTER;

   // Set input file(s). //
   FILE *inputf1 = NULL;
   FILE *inputf2 = NULL;

   // Set output file(s). //
   FILE *outputf1 = NULL;
   FILE *outputf2 = NULL;

   if (input != UNSET) {
      inputf1 = fopen(input, "r");
      if (inputf1 == NULL) {
         fprintf(stderr, "%s cannot open file %s\n", ERRM, input);
         say_usage();
         return EXIT_FAILURE;
      }
   }
   else if (input1 != UNSET) {
      inputf1 = fopen(input1, "r");
      if (inputf1 == NULL) {
         fprintf(stderr, "%s cannot open file %s\n", ERRM, input1);
         say_usage();
         return EXIT_FAILURE;
      }
      inputf2 = fopen(input2, "r");
      if (inputf2 == NULL) {
         fprintf(stderr, "%s cannot open file %s\n", ERRM, input2);
         say_usage();
         return EXIT_FAILURE;
      }
   }
   else {
      inputf1 = stdin;
   }

   if (output != UNSET) {
      outputf1 = fopen(output, "w");
      if (outputf1 == NULL) {
         fprintf(stderr, "%s cannot write to file %s\n", ERRM, output);
         say_usage();
         return EXIT_FAILURE;
      }
   }
   else if (nr_flag && input1 != UNSET && input2 != UNSET) {
      // Set default output file names as inputX-starcode.fastq
      if (output1 == UNSET) {
         output1 = outname(input1);
         outputf1 = fopen(output1, "w");
         free(output1);
         output1 = NULL;
      } else {
         outputf1 = fopen(output1, "w");
      }

      if (outputf1 == NULL) {
         fprintf(stderr, "%s cannot write to output file 1\n", ERRM);
         say_usage();
         return EXIT_FAILURE;
      }

      if (output2 == UNSET) {
         output2 = outname(input2);
         outputf2 = fopen(output2, "w");
         free(output2);
         output2 = NULL;
      } else {
         outputf2 = fopen(output2, "w");
      }

      if (outputf2 == NULL) {
         fprintf(stderr, "%s cannot write to output file 2\n", ERRM);
         say_usage();
         return EXIT_FAILURE;
      }
   }
   else {
      outputf1 = stdout;
   }

   // Set remaining default options.
   if (threads < 0) threads = 1;
   if (cluster_ratio < 0) cluster_ratio = 5;

   if (cluster_ratio == 1.0 && vb_flag) {
      fprintf(stderr, "warning: setting cluster-ratio to 1.0" \
	    " may result in arbitrary cluster breaks.\n");
   }

   int exitcode =
   tailored_clustering(
       inputf1,
       inputf2,
       outputf1,
       outputf2,
       final_dist,
       vb_flag,
       threads,
       cluster_alg,
       cluster_ratio,
       cl_flag,
       id_flag,
       output_type,
       threshold_sub,
       threshold_del,
       threshold_ins,
       org_len,
       dist,
       tail_flag
   );

   if (inputf1 != stdin)   fclose(inputf1);
   if (inputf2 != NULL)    fclose(inputf2);
   if (outputf1 != stdout) fclose(outputf1);
   if (outputf2 != NULL)   fclose(outputf2);

   return exitcode;

}
