
 # Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα (Μέρος 1ο)
   
 ## Μέλη Ομάδας:
 * Ιατροπούλου Σταυρούλα, 1115201500048
 * Ξύδας Μιχαήλ, 1115201500116
 * Στάης Βαλέριος, 1115201500148
                 
 
 ## Task
 http://sigmod18contest.db.in.tum.de/task.shtml
 
On the small dataset best time achieved: 1788ms
  
 ## Compile and Run
Compile: `make` will compile all the executables (unit tests and main executable)  

Run:
 * `make runHarness` runs the contest provided harness with the small dataset
 * `make runInputUt` runs the Input unit test
 * `make runReorderingUt`runs the Hash1 and Reordering tests
 * `make runIndexingUt` runs the Indexing unit tests
 * `make runSearchListUt` runs the Search and List unit test functions


!! In order for input test to be able to pass, `smallTestTables/file3_10, smallTestTables/file3_10ascii` must not be changed` !!  

 ## General optimizations
 * Apply filters before joins
 * Simple utilization of cache locality (like using arrays of pointers to columns instead of pointers to rows)
 
 ## Radix Hash Join 
 The alogorithm which we were given to implement for performing the joins of the queries (which is of curse the main bottleneck).
 * ### Reordering 
	  In the first phase, the h1, we calculate the best n to use, where n is the number of least important bits used as the hash       function. We do this for one of the relations and then use the same n for the other relation.

	We choose which relation to index by taking the largest relation, if their difference in size is between 100.000 and 1.000.000, or the smallest otherwise. We also factor in the number of identical keys in each relation, opting for the one with the most identical keys.

	The best n is calculated simply: Knowing the approximate percentage of identical keys in the relation (IdenticalityTest(relation*)) we start from an ideal value and increase the number of buckets until the rest of the relation is split into buckets that fit in the available cache. The available cache is set to be 20/21 * cache_size, leaving at least 1/21 for the chain and buckets in the indexing phase.
  
 * ### Indexing
    Indexing function initialises and constructs indexArray. Hash2 range for bucket array is computed inside the function taking into account CACHE_SIZE and AVAILABLE_CACHE_SIZE. Empty indexes of hash1keys that don't match with any value of initial column of table are also constructed so as to help searching procedure. If relationIndex structure fits in cache then chain array and bucket array are built through buildIndex and updateChain functions (Note: In order to access next node of the chain from previous node,  chain[i - 1] should be accessed if previous node is buckets[x] = i/chain[x] = i for example). Otherwise, index is separated in "subBuckets" which fit in cache after re-computing hash2range and how many tuples each "subBucket" will contain(eachSize). Each "subBucket" composes the list of the particular hash1key index. 

 * ### Searching
    Searching since it is the most time consuming function aims for the lowest complexity possible. The function finds the start of the chain in O(1) and then it follows it so as every search costs O(chainSize) which is fairly small. Then the result tuples (rowIdR, rowIdS) are saved in 1MB list nodes.  
