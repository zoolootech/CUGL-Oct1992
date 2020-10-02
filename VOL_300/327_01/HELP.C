#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "panel.h"
#include "keys.h"

FILE *handle;
char fn[20];
char *text[500];
char *newln;
char buf[80];
int lines, cur_line;

void process_topic(int);
void move(int);


/*************  main  ***************/

main()
    {
    int topic = 1;
    unsigned aid = 0;

    pan_init();

    pan_activate("helpmenu");

    for (;;)
        {
        pan_put_field("stat", 1,
        "  [Home]=First topic  []=Prior  []=Next  [End]=Last topic  [Enter]=Select");

        pan_field_attr("topic", topic, "pr6");
        aid = pan_get_key();
        pan_field_attr("topic", topic, "ph6");
        switch (aid)
            {
            case UP:
                if (topic > 1)
                    topic--;
                break;
            case DOWN:
                if (topic < 6)
                    topic++;
                break;
            case HOME:
                topic = 1;
                break;
            case END:
                topic = 6;
                break;
            case ENTER:
                if (topic < 6)
                    process_topic(topic);
                else
                    {
                    pan_destroy();
                    exit(0);
                    }
                break;
            }

        }


    }


/*************  process_topic  ***************/

void process_topic(topic)
int topic;
    {
    int i;
    unsigned aid;

    sprintf(fn, "help%d.txt", topic);
    if ((handle = fopen(fn, "r")) == NULL)
        {
        sprintf(buf, "%s - file not found!", fn);
        pan_error(16, __LINE__, buf);
        }

    pan_put_field("stat", 1,
    "        [Esc]=Menu      [PgDn]=Next page      [PgUp]=Prior page  ");

    lines = 0;
    fgets(buf, 80, handle);
    while ((!feof(handle)) && (lines < 500))
        {
        newln = strrchr(buf, '\n');
        if (newln != NULL)
            *newln = 0;

        text[lines] = (char *)malloc(80);
        if (text[lines] == NULL)
            {
            pan_error(16, __LINE__, "Out of memory!");
            }

        strcpy(text[lines], buf);

        lines++;
        fgets(buf, 80, handle);
        }

    fclose(handle);

    pan_activate("helpskel");

    move(cur_line = 0);

    while ((aid = pan_get_key()) != ESC)
        switch (aid)
            {
            case PGDN:
                if (cur_line < lines - 19)
                    cur_line += 20;
                move(cur_line);
                break;
            case PGUP:
                if (cur_line >= 20)
                    cur_line -= 20;
                else
                    if (cur_line > 0)
                        cur_line = 0;
                move(cur_line);
                break;
            case F1:
                if (topic == 3)
                    {
                    pan_activate("helpex1");
                    pan_execute("", 1, 0);
                    pan_destroy();
                    }
                break;
            case F2:
                if (topic == 3)
                   {
                    pan_activate("helpex2");
                    pan_execute("", 1, 0);
                    pan_destroy();
                    }
                break;
            case F3:
                if (topic == 3)
                   {
                    pan_activate("helpex3");
                    pan_get_key();
                    pan_destroy();
                   }
                break;
            }


    pan_destroy();

    for (i=0; i<lines; i++)
        free(text[i]);

    }


/*************  move  ***************/

void move(sub)
int sub;
    {
    int i, j;

    for (i=1; i<=20; i++)
        if ((i+sub) > lines)
            {
            pan_put_field("line", i, "");
            pan_field_attr("line", i, "p7");
            }
        else
            {
            j = i + sub - 1;
            if (strlen(text[j]) > 1)
                {
                if (*text[j] == '+')
                    pan_field_attr("line", i, "ph6");
                else
                    pan_field_attr("line", i, "p7");
                pan_put_field("line", i, text[j]+1);
                }
            else
                {
                pan_put_field("line", i, "");
                pan_field_attr("line", i, "p7");
                }
            }

    }


