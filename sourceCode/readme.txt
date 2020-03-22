Ahhh!!! How it works....
    1.constant space recursive program...pointer to same pathBuffer is passed recursively.
    2.for every access (directory or file).... target is copied into pathBuffer...position of null character is noted
    3.'/' is appended......then new directory entry from directoryIterator->d_name is appended
    4.data accessed
    5.when the said entry truly completely accessed to return to the precursor path null character is placed at the old index from step 2
    6. thus we have bactracked to the target path in the argument in the pathBuffer now.

    P.S.:- For every sub entry that is a sub directory......the pointer to path buffer is passed as target to the next recursive call.
           Only in main, ponter to the buffer setup on mainframe is passed as target to the first call.

           Buffer overflow vulnerability counter measures not enforced in call in main.
           the target buffer 'lol' on main frame could overflow into return address stored in main frame by user functions.
           -Do not know how to enforce it.(programming side of the coin).
           -For every access the end of buffer is checked and maintained to avoid overrun exploit during execution side of the coin.


    > All Errno dodges for opendir/readir are yet to be incorporated
    > only ACESS DENIED exception is handled.
    > comment out the copyToDestination function call in actualTraversal to use the program to just print all the names and traverse.

    >> readList.out just traverses.
    >> pull.out copies all the leaf level files in the heirarchy.
    >> biggest blunder one could do is 
	char cypherChar = getc (fptr); // never cast return value of getc,getchar as a character it would (unofficially mean that every 0xff is EOF) cause a blunder.
    >> EOF is outside the range of characters so its easily distinguished from characters
	int cypherChar = getc (fptr); is correct as the MSB is not dropped off in int container.
    >>getchar,getc return EOF when no more data can be retrieved from a stream...its not necessarily stored as character in the file.

P.P.S.
    Turns out that you can not usually get away with ptr = fopen ("target", rb+);
    it gives errno = 13 i.e. permission denied to modify the contents of file.
    Usually treat a src file as read only to avoid seg fault by int cypherChar = getc (ptr); ptr being NULL gives seg fault.
	>> Two Modes incorporated.