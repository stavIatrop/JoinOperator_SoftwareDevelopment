
 # Join Query Executor (SIGMOD 2018)
   
 ## Collaborators
 * Stavroula Iatropoulou, https://gitlab.com/stavIatrop
 * Valerios Stais, https://gitlab.com/valerios1910
 
 Our approach of dealing with the task follows in many cases the algorithms and general approach of the 3rd placed team Dataurus http://sigmod18contest.db.in.tum.de/leaders.shtml                
 
 ## Task
 http://sigmod18contest.db.in.tum.de/task.shtml
 
On the small dataset best time achieved: 750ms
On the public dataset best time achieved: 50800ms  

Specs  
* CPU: Intel Core i5 7200U 2.5GHz 4 cores
* RAM: 8GB DDR4
* OS: Ubuntu gnome 18.04

Aim of the task was to create an as fast as possible **query executor** accepting only the sql keyword `and`.
For example a query could be:
`SELECT A.1`  
`FROM A, B, C`
`WHERE A.2=B.3 AND A.3=C.2 AND A.2=A.1 AND C.3 >1000`


 ## Compile and Run
Compile: `make` will compile all the executables (unit tests and main executable)  

Run:
 * `make runHarness` runs the contest provided harness with the **small dataset**
 * `make runHarnessPublic` runs the contest provided harness with the **public dataset**. The public dataset since it is big (~350mb) must be manually downloaded from here  http://sigmod18contest.db.in.tum.de/public.tar.gz and be placed and configured like the small dataset which is included in this repo

## General Optimizations

Most of the optimizations and algorithms followed are explained time measured in the report `ProjectReport.pdf`(currently only in Greek).   
A quick list of the optimizations is:
* Create a jobScheduler and apply multithreading on several points of the joiner
* Use of statistics and the join enumeration algorithm to define the most efficient order of executing the joins and filters of the query
* Take into consideration cache sizes and cache utilization
* Use of a "warehouse" of indexes in order to avoid creating them each time if possible
 
 ## Radix Hash Join 
 The alogorithm which we were given to implement for performing the joins of the queries (which is of course the main bottleneck).
 * ### Reordering 
	  In the first phase, the h1, we calculate the best n to use, where n is the number of least important bits used as the hash       function. We do this for one of the relations and then use the same n for the other relation.

	We choose which relation to index by taking the largest relation, if their difference in size is between 100.000 and 1.000.000, or the smallest otherwise. We also factor in the number of identical keys in each relation, opting for the one with the most identical keys.

	The best n is calculated simply: Knowing the approximate percentage of identical keys in the relation (IdenticalityTest(relation*)) we start from an ideal value and increase the number of buckets until the rest of the relation is split into buckets that fit in the available cache. The available cache is set to be 20/21 * cache_size, leaving at least 1/21 for the chain and buckets in the indexing phase.
  
 * ### Indexing
    Indexing function initialises and constructs indexArray. Hash2 range for bucket array is computed inside the function taking into account CACHE_SIZE and AVAILABLE_CACHE_SIZE. Empty indexes of hash1keys that don't match with any value of initial column of table are also constructed so as to help searching procedure. If relationIndex structure fits in cache then chain array and bucket array are built through buildIndex and updateChain functions (Note: In order to access next node of the chain from previous node,  chain[i - 1] should be accessed if previous node is buckets[x] = i/chain[x] = i for example). Otherwise, index is separated in "subBuckets" which fit in cache after re-computing hash2range and how many tuples each "subBucket" will contain(eachSize). Each "subBucket" composes the list of the particular hash1key index. 

 * ### Searching
    Searching since it is the most time consuming function aims for the lowest complexity possible. The function finds the start of the chain in O(1) and then it follows it so as every search costs O(chainSize) which is fairly small. Then the result tuples (rowIdR, rowIdS) are saved in 1MB list nodes.  


## TODOs

* Update the report with the updated faster times (about 40% faster)
* Translate the report in English
