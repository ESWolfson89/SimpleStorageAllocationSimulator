#ifndef BUTTON_H
#define BUTTON_H

class button
{
      public:
         button();
         button(int,int,int,int);
         bool checkClick(int,int);
         int getLocX();
         int getLocY();
         int getSizeX();
         int getSizeY();
      private:
         int locx,locy;
         int sizex,sizey;
};

#endif // BUTTON_H
