/***************************************************************************
*                                                                          *
*                                                                          *
*  W tym pliku jest kod wszystkiego, czeho ktos sam nie napisal tylko po   *
*  prostu skopiowal skads kod bo...					   *
*                                                                          *
* ------------------------------------------------------------------------ *
*                     ____________   __     ______                         *
*   Aldegard    Jot  (   ___   ___| /  \   |  __  )   Thanos      Trog     *
*        ______________\  \|   |  /  ^   \ |     <_______________          *
*        \________________/|___|/___/"\___\|__|\________________/          *
*                  \   \/ \/   //  \   |  __  )(  ___/`                    *
*                    \       //  ^   \ |     <__)  \                       *
*                      \_!_//___/"\___\|__|\______/TM                      *
* (c) 2001, 2002            M       U        D                Ver 1.1      *
****************************************************************************/

#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "mud.h"

//LANG_DATA *		first_lang;
//LANG_DATA *		last_lang;



/* Laczenie przedmiotow w jeden nowy */
DEF_DO_FUN( connect )
{
  OBJ_DATA * first_ob;
  OBJ_DATA * second_ob;
  OBJ_DATA * new_ob = 0;

  char arg1[MAX_STRING_LENGTH], arg2[MAX_STRING_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  if ( arg1[0] == '\0' || arg2[0] == '\0' )
  {
    send_to_char( "Skladnia: combine <pierwszy_przedmiot> <drugi_przedmiot>." NL, ch );
    return;
  }

  if ( ( first_ob =  get_obj_carry( ch, arg1 ) )  == NULL )
  {
    sprintf( buf, "Musisz trzymac obie czesci!!" NL );
    send_to_char( buf, ch );
    return;
  }



  if ( ( second_ob =  get_obj_carry( ch, arg2 ) )  == NULL )
  {
    sprintf( buf, "Musisz trzybac obie czesci!!" NL );
    send_to_char( buf, ch );
    return;
  }

  if ( first_ob->item_type != ITEM_PIECE || second_ob->item_type
!=ITEM_PIECE )
  {
    send_to_char( "Oba przedmioty musza byc czescmi jakiejs rzeczy!" NL,
ch );
    return;
  }

  /* check to see if the pieces connect */
  if (  ( first_ob->value[0] == second_ob->pIndexData->vnum )
     || ( first_ob->value[1] == second_ob->pIndexData->vnum )  )
 /* good connection  */
  {
    if ( first_ob->value[0] == second_ob->pIndexData->vnum && first_ob->value[0] != 0 )
    new_ob = create_object( get_obj_index( first_ob->value[1] ), 0 );
    if ( first_ob->value[1] == second_ob->pIndexData->vnum && first_ob->value[1] != 0 )
    new_ob = create_object( get_obj_index( first_ob->value[2] ), 0 );
    if( !new_ob )
    {
        bug( "Connect: no new_ob, check objects values.", 0 );
        return;
    }
    extract_obj( first_ob );
    extract_obj( second_ob );
    obj_to_char( new_ob , ch );
    act( COL_ACTION, "$n probuje zlaczyc jakies dwa przedmioty i... udaje sie.\n\rZ polaczenia powstaje: $p!", ch, new_ob,NULL, TO_ROOM );
    act( COL_ACTION, "Probujesz zlaczyc jakies dwa przedmioty i... udaje sie.\n\rZ polaczenia powstaje: $p!", ch, new_ob, NULL, TO_CHAR);

  }
  else
  {
    act( COL_ACTION, "$n Probuje polaczyc jakies dwa przedmioty ale one kompletnie do siebie nie pasuja.", ch, NULL, NULL, TO_ROOM);
    act( COL_ACTION, "Probujesz polaczyc jakies dwa przedmioty ale one kompletnie do siebie nie pasuja.", ch, NULL, NULL, TO_CHAR);
  

  //  return;
  }

  return;
}

/* Laczenie przedmiotow ... koniec. */



/* Za�atwi�em fajne j�zyki ze smauga				-Thanos*/
/*
 * Tongues / Languages loading/saving functions			-Altrag
 */
void fread_cnv(FILE * fp, std::list<LCNV_DATA*>& cnv_list)
{
    LCNV_DATA *cnv;
    char letter;

    for (;;)
    {
	letter = fread_letter(fp);
	if (letter == '~' || letter == EOF)
	    break;
	ungetc(letter, fp);
	CREATE(cnv, LCNV_DATA, 1);

	STRDUP( cnv->old, fread_word(fp) );
	cnv->olen = strlen(cnv->old);
	STRDUP( cnv->_new, fread_word(fp) );
	cnv->nlen = strlen(cnv->_new);
	fread_to_eol(fp);
	cnv_list.push_back(cnv);
    }
}

void load_tongues()
{
    FILE *fp;
    LANG_DATA *lng;
    char *word;
    char letter;

    if (!(fp=fopen(TONGUE_FILE, "r")))
    {
	perror("Load_tongues");
	return;
    }
    for (;;)
    {
	letter = fread_letter(fp);
	if (letter == EOF)
	{
	    fclose( fp );
	    return;
	}

	else if (letter == '*')
	{
	    fread_to_eol(fp);
	    continue;
	}
	else if (letter != '#')
	{
	    bug("Letter '%c' not #.", letter);
	    exit(0);
	}
	word = fread_word(fp);
	if (!str_cmp(word, "end"))
	{
	    fclose( fp );
	    return;
	}
	fread_to_eol(fp);
	CREATE(lng, LANG_DATA, 1);
	STRDUP(lng->name,word);
	fread_cnv(fp, lng->precnv);
	lng->alphabet = fread_string(fp);
	fread_cnv(fp, lng->cnv);
	fread_to_eol(fp);
	lang_list.push_back(lng);
    }
    fclose( fp );
    return;
}

void fwrite_langs(void)
{
    FILE *fp;

    if (!(fp=fopen(TONGUE_FILE, "w")))
    {
	perror("fwrite_langs");
	return;
    }
    for (auto* lng : lang_list)
    {
	fprintf(fp, "#%s\n", lng->name);
	for (auto* cnv : lng->precnv)
	    fprintf(fp, "'%s' '%s'\n", cnv->old, cnv->_new);
	fprintf(fp, "~\n%s~\n", lng->alphabet);
	for (auto* cnv : lng->cnv)
	    fprintf(fp, "'%s' '%s'\n", cnv->old, cnv->_new);
	fprintf(fp, "\n");
    }
    fprintf(fp, "#end\n\n");
    fclose(fp);
    return;
}









