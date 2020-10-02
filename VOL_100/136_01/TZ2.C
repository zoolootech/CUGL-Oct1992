/*	HEADER:  CUG136.02;
	TITLE:		TZ-Part 2;
	VERSION:	0.29;
	DATE:		3/12/1986;
	SYSTEM:		CP/M,MS-DOS,TOS;
	FILENAME:	TZ2.C;
	AUTHORS:	R. Rodman;
	COMPILERS:	C/80,Aztec,Alcyon;
*/

/* TZ2.C - Miscellaneous parts of TZ game.
	860311 rr removed from TZ.C
	860319 rr add st extra delay, message */

extern int rm_num, nu_fl, cratct, beerct, wtchct, pushct, pushfl, noun;
extern int answer[ 5 ];

extern print();

hello()
{
	print( "*You unlock this door with the key of imagination." );
	ilpdel();
	print( "\\Beyond it is another dimension..." );
	ilpdel();
	print( "\\    ...a dimension of sound..." );
	ilpdel();
	print( "\\       ...a dimension of sight..." );
	ilpdel();
	print( "\\          ...a dimension of mind..." );
	ilpdel();
	print( "^You're moving into a land of both shadow and substance..." );
	ilpdel();
	print( "\\    ...of things and ideas..." );
	ilpdel();
	print( "\\    ...You've just crossed over..." );
	ilpdel();
	print( "\\\\          ...into the Twilight Zone." );
}

/* Long Delay */

ilpdel()
{
	int k;
	for( k = 1; k <= 30000; k++ );
#ifdef ST
	for( k = 1; k <= 30000; k++ );
	for( k = 1; k <= 30000; k++ );
#endif
}

/* Examine Bed */

exbed()
{
	print( "^What are beds for?" );
}

/* Marsh */

pool()
{
	print( "^You see yourself looking down into the pool..." );
	print( " Could it mean that YOU are the MARSH-IAN?" );
}

echh()
{
	print( "^ECHH! Tastes AWFUL!" );
}

/* Phone Booths */

busy()
{
	print( "^The phone is busy." );
}

call7()
{
	rm_num = 7;
	nu_fl = 1;
}

call11()
{
	rm_num = 11;
	nu_fl = 1;
}

sign()
{
	print( "^'No dime needed to call operator'." );
}

book()
{
	print( "^'Let your fingers do the walking ...'" );
}

/* Safe (a Red Herring) */

opsafe()
{
	print( "^It's locked.If you knew the combination,you" );
	print( " could dial it." );
}

dlcomb()
{
	print( "^The heavy steel door of the safe swings open," );
	print( " revealing...nothing,save a small piece of paper." );
}

paper( r )
int r;
{
	switch( r ) {
		case 86 :
print( "^The paper says simply,'Combination: 22-41-66.'" );
			break;
		case 152 : print( "^'Thought it would be that easy?'" );
			break;
		default : cant();
	}
}

form()
{
	print( "^The Racing Form is a newspaper-like publication" );
	print( " which lists local horse races and shows calculated" );
	print( " odds on each horse.Gamblers often read these" );
	print( " before placing bets." );
	print( "\\There is an inkblot on this form,which appears" );
	print( " to have inadvertantly dripped from the pen." );
}

blot()
{
	print( "^It looks like a crab,or maybe,..." );
	print( "\Anyway,the blot has landed next to a horse" );
	print( " in the third race named 'Royal _3'.Might be a " );
	print( "good bet!" );
}

/* Mirrors */

seeslf()
{
	print( "^You see yourself standing in the hotel room." );
}

seecow()
{
	print( "^You see yourself standing in the hotel room --" );
	print( " but dressed up in dusty Old West clothing!" );
}

mirhse( i )
int i;
{
	print( "^You see yourself in a state of" );
	switch( i ) {
		case 223 : print( " confidence." ); break;
		case 224 : print( " frustration." ); break;
		case 225 : print( " thought." ); break;
		case 226 : print( " indecision." ); break;
		case 227 : print( " desperation." ); break;
		case 228 : print( " anxiety." ); break;
		case 229 : print( " reflection." ); break;
		case 230 : print( " agitation." ); break;
		case 231 : print( " surprise!" ); break;
	}
}

menu( r )
int r;
{
	switch( r ) {
		case 9 : print( "^'House Specialties:" );
print( "\\Spare Ribs ...6.98     Red Herring ...5.00" );
print( "\\Food for Thought:" );
print( " You must find something in this world and take" );
print( " it somewhere.Watch for clues as to what it is." );
print( " Remember,Information is the most valuable of" );
print( " all possessions.'" );
			break;
		case 221 : print( "^'House Specialties:" );
print( "\\Spare Ribs ...6.98    Blue Trout ...2.65" );
print( "\\Food for Thought: Read the Plate." );
			break;
		default: print( "^Don't see one here." );
	}
}

drwatr()
{
	print( "^Ahh...you were getting thirsty!" );
}

/* Crate */

excrat()
{
	if (cratct == 0 )
		print( "^It looks as though it could be opened..." );
	else {
		print(  "^Very odd indeed! The crate has a smaller" );
		print( " crate inside it ...\\within that one is a" );
		print( " ...wax hand? Maybe part of a statue?" );
	}
}

opcrat()
{
	if( ++cratct > 1 )
		print( "^It's already open!" );
	else
		print( "^"); 
}

writing( r )
int r;
{
	switch( r ) {
		case 21 : print( "^'Buy a beer at the saloon.'" );
			break;
		case 29 : inst(); break;	
		case 214 : print( "^'The Twilight Zone Game is" );
			print( " Copyright (C) 1982,1983 by R.Rodman.'" );
			break;
		default : print( "^Don't see any." );
	}
}

inst()
{
	print( "^'The key to the Zone is a multi-digit code.Once having" );
	print( " determined it,you may leave the Zone if you use it in" );
	print( " the proper way.Be careful not to disclose it to the" );
	print( " wrong person!'" );
}

/* Buy Beer */

buyber()
{
	switch( ++beerct ) {
		case 1 :
			print( "^You strike up a conversation with the" );
			print( " bartender.\'Let me give you a hint,' he" );
			print( " says.'When you get to Centerville," );
			print( " take a ride on the train.'" );
			print( " He then goes back to washing glasses." );
			break;
		default :
			print( "^Without saying a word,he slides a glass" );
			print( " of beer over to you." );
	}
}

drbeer()
{
	print( "^Ahh...Really hits the spot!" );
}

ridhor()
{
	print( "^The horse won't go anywhere,no matter what you do." );
}

taktrn()
{
	print( "^The train pulls up and you get on.It rolls out and" );
	print( " travels past the Centerville water tower.Then,it" );
	print( " rolls past five abandoned automobiles.The second one" );
	print( " looks like an old race car; it is painted a strange" );
	print( " _2 color,and on it is a number 6." );
	print( " The train continues past some cornfields" );
	print( " and crosses the gravel road entering Centerville." );
	print( " Finally,it arrives back at the station again." );
}

taknot()
{
	print( "^You can't take it with you." );
}

/* Read License Plates */

plates( i )
int i;
{
	switch( i ) {
		case 9 :
		case 221 :
print( "^There is nothing written on it.Perhaps a different kind of plate" );
print( " might prove more interesting!" );
			break;
		case 205 : print( "^'B46 317'." ); break;
		case 206 : print( "^'GJJ-165'." ); break;
		case 207 : print( "^'301 BFJ'." ); break;
		case 208 : print( "^'SQR-602'." ); break;
		case 209 : print( "^'F65-G37'." ); break;
		case 210 : print( "^'1225348'." ); break;
		case 211 : print( "^'421 JK'." ); break;
		case 212 : print( "^'JKL-123'." ); break;
		case 213 : print( "^'DIGIT 5'." ); break;
		default : cant(); break;
	}
}

opgrat()
{
	print( "^Oh boy! Maybe there's a stairway leading to a" );
	print( " dome,and a cage,and a bird,and a rusty black" );
	print( " rod with a star on the end!" );
	print( " \...Nuts,it won't open." );
}

/* Watch TV */

examtv()
{
	print( "^It is set to channel 10." );
}

wtchtv()
{
	if( wtchct < 5 ) wtchct++;
	switch( wtchct ) {
		case 1 : {
			print( "^There are slanted,almost horizontal " );
			print( "lines on the screen,as though the " );
			print( "set is out of repair.The sound is garbled," );
			print( "apparently a commercial." );
			break;
		}
		case 2 : {
			print( "^The picture has cleared up and now shows " );
			print( "YOU! You are walking across a desert past " );
			print( "an old car." );
			break;
		}
		case 3 : {
			print( "^The screen shows you near a burned-out " );
			print( "place in the desert,crouched over a " );
			print( "crumpled body." );
			break;
		}
		case 4 : {
			print( "^The screen shows you closely examining a " );
			print( "hand,as though there were something odd " );
			print( "about it!" );
			break;
		}
		case 5 : {
			print( "^The screen is filled with slanted,almost " );
			print( "horizontal lines." );
			break;
		}
	}
}

exhand()
{
	print( "^The hand is wearing a bright _1 ring on its index" );
	print( " finger." );
}

/* push button */

push()
{
	if( noun < 32 || noun > 36 ) {
		cant();
		return;
	}

	if( noun - 32 != answer[ pushct++ ] ) pushfl = 1;

	if( pushct < 5 ) {
		print( "^There is a faint click." );
		return;
	}

	if( pushfl ) {
		pushct = 0;		/* incorrect,reset everything */
		pushfl = 0;
		print( "^There is a beeping sound." );
		return;
	}

	print( "*A blinding light surrounds you momentarily,and" );
	print( " when it subsides,you find yourself in a beautiful" );
	print( " meadow.Children play nearby,and the sun glints" );
	print( " off the snow on rugged mountain peaks in the far" );
	print( " distance.You have escaped the Zone and returned" );
	print( " to your own dimension.Enjoy it!" );
	exit( 0 );
}

/* end of tz2.c */


