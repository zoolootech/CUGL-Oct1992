#include <stdio.h>
#include <graphics.h>

main()
{
    int driver,mode;

    driver=VGA;
    mode=VGAMED;
    initgraph(&driver,&mode,"");
    file_image(250,150,"dom-43.pli");
    file_image(200,150,"dom-64.pli");
    file_image(175,139,"dom-66.pli");
    file_image(125,150,"dom-26.pli");
    file_image(300,150,"dom-31.pli");
    file_image(350,139,"dom-11.pli");
    file_image(375,150,"dom-15.pli");
    file_image(125,110,"dom-02.pli");
    file_image(125,168,"dom-24.pli");
    file_image(113,208,"dom-44.pli");
    file_image(125,226,"dom-45.pli");
    file_image(113, 92,"dom-00.pli");
    file_image(125, 52,"dom-60.pli");
    file_image(400,168,"dom-52.pli");
    file_image(400,110,"dom-05.pli");
    setbkcolor(1);
    getch();
    restorecrtmode();
    return;
}
