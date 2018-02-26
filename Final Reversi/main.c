//
//  main.c
//  Reversi
//  Brain Code
//  Created by 1750114 on 2018/01/05.
//

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#define NONE 0
#define BLACK 1
#define WHITE 2
#define SPAN 16
#define DEPTH 4

//Gameboard
int iBoard[SPAN][SPAN];
//Game Info
int iMyside=0;
int iEneside=0;
char chGamestatus[10];
int iBoardAll=4;
//Alpha-beta related
int iHistory[2][SPAN*SPAN][SPAN*SPAN];
//QuietMove Evaluation related
int iDiv[SPAN][SPAN];

//Topographical Evaluation related
int iTopoBoard[SPAN][SPAN]={
    {370,-3000,350,300,300,290,290,290,290,290,290,300,300,350,-3000,370},
    {-3000,-300,-200,-200, 10, 10, 10, 10, 10, 10, 10, 10,-200,-200,-300,-3000},
    {350,-200,200,150,150,110,110,110,110,110,110,150,150,200,-200,350},
    {300,-200,150, 70, 70, 85, 85, 85, 85, 85, 85, 70, 70,150,-200,300},
    {300, 10,150, 70, 70, 85, 85, 85, 85, 85, 85, 70, 70,150, 10,300},
    {290, 10,110, 85, 85, 82, 82, 82, 82, 82, 82, 85, 85,110, 10,290},
    {290, 10,110, 85, 85, 82, 81, 81, 81, 81, 82, 85, 85,110, 10,290},
    {290, 10,110, 85, 85, 82, 81, 81, 81, 81, 82, 85, 85,110, 10,290},
    {290, 10,110, 85, 85, 82, 81, 81, 81, 81, 82, 85, 85,110, 10,290},
    {290, 10,110, 85, 85, 82, 81, 81, 81, 81, 82, 85, 85,110, 10,290},
    {290, 10,110, 85, 85, 82, 82, 82, 82, 82, 82, 85, 85,110, 10,290},
    {300,-200,150, 70, 70, 85, 85, 85, 85, 85, 85, 70, 70,150,10,300},
    {300,-200,150, 70, 70, 85, 85, 85, 85, 85, 85, 70, 70,150,-200,300},
    {350,-200,200,150,150,110,110,110,110,110,110,150,150,200,-200,350},
    {-3000,-300,-200,-200, 10, 10, 10, 10, 10, 10, 10, 10,-200,-200,-300,-3000},
    {370,-3000,350,300,300,290,290,290,290,290,290,300,300,350,-3000,370},
};

/*BASIC FUNCTIONS*/

//Used to Add +or- to Evaluation Results  My:+ ; Ene:-
//Parameters:Side Changed:None
int fnSign(int iSide){
    if(iSide==iMyside){
        return 1;
    }
    else{
        return -1;
    }
}

//Function for Changing in Specific Direction
//Parameters:iRow,iCol  Changed:iRow,iCol
void U(int *pRow,int *pCol){
    *pRow-=1;
}

void UL(int *pRow,int *pCol){
    *pCol-=1;
    *pRow-=1;
}

void L(int *pRow,int *pCol){
    *pCol-=1;
}

void DL(int *pRow,int *pCol){
    *pCol-=1;
    *pRow+=1;
}

void D(int *pRow,int *pCol){
    *pRow+=1;
}

void DR(int *pRow,int *pCol){
    *pCol+=1;
    *pRow+=1;
}

void R(int *pRow,int *pCol){
    *pCol+=1;
}

void UR(int *pRow,int *pCol){
    *pCol+=1;
    *pRow-=1;
}

//Check if a specific Row & Column goes out of the Limits of Gameboard
//Parameters: iRow,iCol    Changed:None
//Return:The Result of Check  Type:Boolean
bool fnValid(int iRow,int iCol){
    if(iRow<0||iRow>SPAN-1||iCol>SPAN-1||iCol<0){
        return false;
    }
    else{
        return true;
    }
}

//Count the Number of Reversible Chess of One Direction
//Parameters:iRow,iCol,Side awaiting to Place, Direction  Changed:None
//Return:Number   Type:int
int fnRevcount(int iRow,int iCol,int iSide,void (*direction)(int *,int *)){
    int iTurn=0;
    int i=iRow;
    int j=iCol;
    (direction)(&i,&j);
    if(iBoard[i][j]==NONE||iBoard[i][j]==iSide){
        iTurn=0;
    }
    else{
        while(fnValid(i,j)&&iBoard[i][j]!=iSide&&iBoard[i][j]!=NONE){
            iTurn++;
            (direction)(&i,&j);
        }
        if(!fnValid(i,j)){
            iTurn=0;
        }
        else if(iBoard[i][j]==NONE){
            iTurn=0;
        }
    }
    return iTurn;
}

//Judge whether a specific Placement is Valid
//Parameters:iRow,iCol,Side awaiting to Place  Changed:None
//Return:Availability of the Placement   Type:Boolean
bool fnAblecheck(int iRow,int iCol,int iSide){
    if(iBoard[iRow][iCol]!=NONE){
        return false;
    }
    if(fnRevcount(iRow,iCol,iSide,U)>=1
       ||fnRevcount(iRow,iCol,iSide,UL)>=1
       ||fnRevcount(iRow,iCol,iSide,L)>=1
       ||fnRevcount(iRow,iCol,iSide,DL)>=1
       ||fnRevcount(iRow,iCol,iSide,D)>=1
       ||fnRevcount(iRow,iCol,iSide,DR)>=1
       ||fnRevcount(iRow,iCol,iSide,R)>=1
       ||fnRevcount(iRow,iCol,iSide,UR)>=1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Count the Available Moves of one specific Side
//Parameters:Side    Changed:None
//Return:The Number of Available Moves   Type:int
int fnAVAmoves(int iSide){
    int iRow;int iCol;int iCount=0;
    for(iRow=0;iRow<=SPAN-1;iRow++){
        for(iCol=0;iCol<=SPAN-1;iCol++){
            if(fnAblecheck(iRow, iCol, iSide)){
                iCount++;
            }
        }
    }
    return iCount;
}

//Recount Div for a specific Place(Row,Col)
//Parameters:iRow,iCol    Changed:iDiv
void fnFlipDivTrueO(int iRow,int iCol){
    int iTR,iTC,iDivC=8;
    
    iTR=iRow;iTC=iCol;
    L(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    UL(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    U(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    UR(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    R(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    DR(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    D(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    
    iTR=iRow;iTC=iCol;
    DL(&iTR,&iTC);
    if(iBoard[iTR][iTC]!=NONE){
        iDivC--;
    }
    iDiv[iRow][iCol]=iDivC;
}

//Store Simulated Reverse Data(Chess)  Flip WareHouse
int iU[DEPTH],iUL[DEPTH],iL[DEPTH],iDL[DEPTH],iD[DEPTH],iDR[DEPTH],iR[DEPTH],iUR[DEPTH];
//Simulated Flip Chess(iBoard) and QuietEVABoard(iDiv) for one Direction
//Parameters:iRow,iCol,Side,Direction(pointer)    Changed:iBoard,iDiv
//Return:The Number of Chess flipped   Type:int
int fnFlipone(int iRow,int iCol,int iSide,void (*direction)(int *,int *)){
    //Flip Board(Chess)
    int iTurn=fnRevcount(iRow,iCol,iSide,direction);
    int iR=iRow;
    int iC=iCol;
    int iCount=iTurn;
    if(iCount>=1){
        for(;iCount>0;iCount--){
            (direction)(&iR,&iC);
            iBoard[iR][iC]=iSide;
        }
    }
    //Flip Div
    iR=iRow;
    iC=iCol;
    iCount=iTurn;
    if(iCount>=1){
        for(;iCount>0;iCount--){
            (direction)(&iR,&iC);
            fnFlipDivTrueO(iR, iC);
        }
    }
    return iTurn;
}

//Simulated UnFlip QuietEVABoard(iDiv) for one Direction
//Parameters:iRow,iCol,Side,Direction(pointer),iCount(iTurn)    Changed:iDiv
void fnUnflipDivone(int iRow,int iCol,void (*direction)(int *,int *),int iCount){
    int iR=iRow;
    int iC=iCol;
    if(iCount>=1){
        for(;iCount>0;iCount--){
            (direction)(&iR,&iC);
            fnFlipDivTrueO(iR, iC);
        }
    }
}

//Simulated Flip Chess(iBoard) and QuietEVABoard(iDiv)(8 Directions)
//Parameters:iRow,iCol,Side,Depth(current)    Changed:iBoard,iDiv
void fnFlip(int iRow,int iCol,int iSide,int depth){
    iBoard[iRow][iCol]=iSide;
    iU[depth-1]=fnFlipone(iRow,iCol,iSide,U);
    iUL[depth-1]=fnFlipone(iRow,iCol,iSide,UL);
    iL[depth-1]=fnFlipone(iRow,iCol,iSide,L);
    iDL[depth-1]=fnFlipone(iRow,iCol,iSide,DL);
    iD[depth-1]=fnFlipone(iRow,iCol,iSide,D);
    iDR[depth-1]=fnFlipone(iRow,iCol,iSide,DR);
    iR[depth-1]=fnFlipone(iRow,iCol,iSide,R);
    iUR[depth-1]=fnFlipone(iRow,iCol,iSide,UR);
    fnFlipDivTrueO(iRow, iCol);
}

//Simulated UnFlip Chess(iBoard) of one Direction
//Parameters:iRow,iCol,Side,Direction,Depth(current),Flip WareHouse(pointer)    Changed:iBoard
void fnUnFlipone(int iRow,int iCol,int iSide,void (*direction)(int *,int *),int depth,int *p){
    int iR=iRow,iC=iCol;
    int iTurn=*(p+depth-1);
    if(iSide==WHITE){
        for(;iTurn>0;iTurn--){
            (direction)(&iR,&iC);
            iBoard[iR][iC]=BLACK;
        }
    }
    else if(iSide==BLACK){
        for(;iTurn>0;iTurn--){
            (direction)(&iR,&iC);
            iBoard[iR][iC]=WHITE;
        }
    }
}

//Simulated UnFlip Chess(iBoard) and QuietEVABoard(iDiv)(8 Directions)
//Parameters:iRow,iCol,Side,Depth(current)    Changed:iBoard,iDiv
void fnUnFlip(int iRow,int iCol,int iSide,int depth){
    //UnFlip Chess
    iBoard[iRow][iCol]=NONE;
    fnUnFlipone(iRow,iCol,iSide,U,depth,iU);
    fnUnFlipone(iRow,iCol,iSide,UL,depth,iUL);
    fnUnFlipone(iRow,iCol,iSide,L,depth,iL);
    fnUnFlipone(iRow,iCol,iSide,DL,depth,iDL);
    fnUnFlipone(iRow,iCol,iSide,D,depth,iD);
    fnUnFlipone(iRow,iCol,iSide,DR,depth,iDR);
    fnUnFlipone(iRow,iCol,iSide,R,depth,iR);
    fnUnFlipone(iRow,iCol,iSide,UR,depth,iUR);
    //UnFlip iDiv
    fnUnflipDivone(iRow, iCol, U, iU[depth-1]);
    fnUnflipDivone(iRow, iCol, UL, iUL[depth-1]);
    fnUnflipDivone(iRow, iCol, L, iL[depth-1]);
    fnUnflipDivone(iRow, iCol, DL, iDL[depth-1]);
    fnUnflipDivone(iRow, iCol, D, iD[depth-1]);
    fnUnflipDivone(iRow, iCol, DR, iDR[depth-1]);
    fnUnflipDivone(iRow, iCol, R, iR[depth-1]);
    fnUnflipDivone(iRow, iCol, UR, iUR[depth-1]);
    iDiv[iRow][iCol]=-1;
}

//True Flip Chess(iBoard) and QuietEVABoard(iDiv)(8 Directions)
//Parameters:iRow,iCol,Side    Changed:iBoard,iDiv
void fnFlipTrue(int iRow,int iCol,int iSide){
    iBoard[iRow][iCol]=iSide;
    fnFlipone(iRow,iCol,iSide,U);
    fnFlipone(iRow,iCol,iSide,UL);
    fnFlipone(iRow,iCol,iSide,L);
    fnFlipone(iRow,iCol,iSide,DL);
    fnFlipone(iRow,iCol,iSide,D);
    fnFlipone(iRow,iCol,iSide,DR);
    fnFlipone(iRow,iCol,iSide,R);
    fnFlipone(iRow,iCol,iSide,UR);
    fnFlipDivTrueO(iRow, iCol);
}


/*EVALUATION(EVA)*/

//Enhance Corner and Side Performance of Later Phase
//Parameters:iSide  Changed:None
//Return:EVA result   Type:int
int fnCorEVAnSuff(int iSide){
    int iTotal=0,iRow,iCol;
    iRow=0;
    iCol=SPAN-1;
    while(fnValid(iRow,iCol)&&iBoard[iRow][iCol]==iSide){
        if(iRow==0&&iCol==SPAN-1){
            iTotal=iTotal+10000;
        }
        else{
            iTotal+=iTotal+3000;
        }
        L(&iRow,&iCol);
    }
    iRow=0;
    iCol=0;
    while(fnValid(iRow,iCol)&&iBoard[iRow][iCol]==iSide){
        if(iRow==0&&iCol==0){
            iTotal=iTotal+10000;
        }
        else{
            iTotal+=iTotal+3000;
        }
        D(&iRow,&iCol);
    }
    iRow=SPAN-1;
    iCol=0;
    while(fnValid(iRow,iCol)&&iBoard[iRow][iCol]==iSide){
        if(iRow==SPAN-1&&iCol==0){
            iTotal=iTotal+10000;
        }
        else{
            iTotal+=iTotal+3000;
        }
        R(&iRow,&iCol);
    }
    iRow=SPAN-1;
    iCol=SPAN-1;
    while(fnValid(iRow,iCol)&&iBoard[iRow][iCol]==iSide){
        if(iRow==SPAN-1&&iCol==SPAN-1){
            iTotal=iTotal+10000;
        }
        else{
            iTotal+=iTotal+3000;
        }
        U(&iRow,&iCol);
    }
    return iTotal;
}

//Enhance Corner Performance of Later Phase (Small Supplement) Real Corner
//Parameters:Side,iRow,iCol  Changed:None
//Return:EVA result   Type:int
int fnCornerS(int iSide,int iRow,int iCol){
    int iCount=0;
    if(iRow==0&&iCol==0&&fnAblecheck(iRow, iCol, iSide)){
        iCount++;
    }
    if(iRow==SPAN-1&&iCol==0&&fnAblecheck(iRow, iCol, iSide)){
        iCount++;
    }
    if(iRow==0&&iCol==SPAN-1&&fnAblecheck(iRow, iCol, iSide)){
        iCount++;
    }
    if(iRow==SPAN-1&&iCol==SPAN-1&&fnAblecheck(iRow, iCol, iSide)){
        iCount++;
    }
    return iCount*20000;
}

//EVA QuietMoves for one direction
//Parameters:iRow,iCol,Side,Direction  Changed:None
//Return:EVA result   Type:int
int fnEVADivone(int iRow,int iCol,int iSide,void (*direction)(int *,int *)){
    int iTotal=0;
    int iTurn=fnRevcount(iRow,iCol,iSide,direction);
    int iR=iRow;
    int iC=iCol;
    int iCount=iTurn;
    if(iCount>=1){
        for(;iCount>0;iCount--){
            (direction)(&iR,&iC);
            iTotal+=iDiv[iR][iC];
        }
    }
    return iTotal;
}

//EVA QuietMoves(8 Directions)    More==Worse
//Parameters:iRow,iCol,Side  Changed:None
//Return:EVA result   Type:int
int fnEVAQuiet(int iRow,int iCol,int iSide){
    int iTotal=0;
    iTotal+=fnEVADivone(iRow,iCol,iSide,U);
    iTotal+=fnEVADivone(iRow,iCol,iSide,UL);
    iTotal+=fnEVADivone(iRow,iCol,iSide,L);
    iTotal+=fnEVADivone(iRow,iCol,iSide,DL);
    iTotal+=fnEVADivone(iRow,iCol,iSide,D);
    iTotal+=fnEVADivone(iRow,iCol,iSide,DR);
    iTotal+=fnEVADivone(iRow,iCol,iSide,R);
    iTotal+=fnEVADivone(iRow,iCol,iSide,UR);
    return -iTotal;
}

//Leaf Node EVA(EVAs Included:Corner&Side,CornerS,Topo)
//Parameters:None  Changed:None
//Return:EVA result   Type:int
int fnEVA(){
    int iTotal=0;
    int iRow,iCol,iAVAMy=0,iAVAEne=0,iMy=0,iReturnMy;
    int iCorner;
    for(iRow=0;iRow<=SPAN-1;iRow++){
        for(iCol=0;iCol<=SPAN-1;iCol++){
            if(iBoard[iRow][iCol]==iMyside){
                iTotal+=iTopoBoard[iRow][iCol]*fnSign(iMyside);
                iMy++;
            }
            else if(iBoard[iRow][iCol]==iEneside){
                iTotal+=iTopoBoard[iRow][iCol]*fnSign(iEneside);
            }
            if(fnAblecheck(iRow, iCol, iMyside)){
                iAVAMy++;
            }
            if(fnAblecheck(iRow,iCol,iEneside)){
                iAVAEne++;
            }
            iCorner=fnCornerS(iMyside,iRow,iCol)+fnCornerS(iEneside,iRow,iCol)*fnSign(iEneside);
            iTotal=iTotal+iCorner;
        }
    }
    //Dangerous
    int iCornerEVA=fnCorEVAnSuff(iMyside)-fnCorEVAnSuff(iEneside);
    if(iMy<=5&&iBoardAll>=20){
        iReturnMy=-3000;
    }
    else{
        iReturnMy=0;
    }
    return iTotal/10+(iAVAMy-iAVAEne)*50+iCornerEVA+iReturnMy;
}

/*ALPHA-BETA PRUNING*/

//Sign Function of Experience Array  WHITE:1;BLACK:0
//Parameters:Side  Changed:None
//Return:Represent Number of a Side   Type:int
int fnExpSpecial(int iSide){
    if(iSide==WHITE){
        return 1;
    }
    else{
        return 0;
    }
}

//Move one Experience to the Top(Best)
//Parameters:Side,Steps,Position  Changed:iHistory
void fnExpTop(int iSide,int iStep,int iPos){
    int iPosC,iMoveTemp;
    if(iPos>0){
        iMoveTemp=iHistory[fnExpSpecial(iSide)][iStep][iPos];
        for(iPosC=iPos-1;iPosC>=0;iPosC--){
            iHistory[fnExpSpecial(iSide)][iStep][iPosC+1]=iHistory[fnExpSpecial(iSide)][iStep][iPosC];
        }
        iHistory[fnExpSpecial(iSide)][iStep][0]=iMoveTemp;
    }
}

//Exchange one Experience with the previous one(Better)
//Parameters:Side,Steps,Position  Changed:iHistory
void fnExpSChange(int iSide,int iStep,int iPos){
    int iMoveTemp;
    iMoveTemp=iHistory[fnExpSpecial(iSide)][iStep][iPos-1];
    if(iPos>0){
        iHistory[fnExpSpecial(iSide)][iStep][iPos-1]=iHistory[fnExpSpecial(iSide)][iStep][iPos];
        iHistory[fnExpSpecial(iSide)][iStep][iPos]=iMoveTemp;
    }
}

//Store Best Moves&Max
int iMaxRow=-1,iMaxCol=-1;int iMax=-INT_MAX;
//Alpha-Beta Pruning
//Parameters:Alpha,Beta,Side,Depth  Changed:iHistory
//Return:Max  Type:int
int fnABP(int alpha,int beta,int iSide,int depth){
    int iVal=0;
    int iDivEVA=0;
    if(depth<=0){
        return fnSign(iSide)*fnEVA();
    }
    if(fnAVAmoves(iSide)==0){
        if(fnAVAmoves(iMyside+iEneside-iSide)==0){
            return fnSign(iSide)*fnEVA();
        }
        return -fnABP(-beta,-alpha,iMyside+iEneside-iSide,depth);
    }
    int iRow,iCol,iMaxAuto=-INT_MAX,iHistoryC;
    for(iHistoryC=0;iHistoryC<=SPAN*SPAN-1;iHistoryC++){
        iRow=iHistory[fnExpSpecial(iSide)][iBoardAll+DEPTH-depth][iHistoryC]/1000;
        iCol=iHistory[fnExpSpecial(iSide)][iBoardAll+DEPTH-depth][iHistoryC]-iRow*1000;
        if(fnAblecheck(iRow,iCol,iSide)){
            iDivEVA=fnEVAQuiet(iRow,iCol,iSide);
            fnFlip(iRow, iCol, iSide,depth);
            iVal=-fnABP(-beta, -alpha,iMyside+iEneside-iSide, depth-1)+iDivEVA*10;
            fnUnFlip(iRow, iCol,iSide,depth);
            if(iVal>alpha){
                if(iVal>=beta){
                    fnExpTop(iSide, iBoardAll+DEPTH-depth,iHistoryC);
                    return iVal;
                }
                fnExpSChange(iSide,iBoardAll+DEPTH-depth,iHistoryC);
                alpha=iVal;
            }
            if(iVal>iMaxAuto){
                iMaxAuto=iVal;
            }
            if(iMaxAuto==iVal&&depth==DEPTH){
                iMax=iVal;
            }
            if(iVal==iMax&&depth==DEPTH){
                iMaxRow=iRow;
                iMaxCol=iCol;
            }
        }
    }
    if(depth==DEPTH){
        return iMax;
    }
    else{
        return iMaxAuto;
    }
}

/*COMMUNICATION WITH THE PLATFORM*/

//Place the Chess(the Result of ALPHA-BETA PRUNING)
//Changed:None
void fnPlace(){
    //初始化
    iMaxRow=-1;iMaxCol=-1;iMax=-INT_MAX;
    int alpha=-INT_MAX;int beta=INT_MAX;
    int max;
    max=fnABP(alpha, beta, iMyside, DEPTH);
    printf("%d %d\n",iMaxRow,iMaxCol);
    fflush(stdout);
    iBoardAll++;
    fnFlipTrue(iMaxRow, iMaxCol, iMyside);
}

//Reply to the Stream of Stdin
//Changed:None
void fnReply(){
    if(strcmp(chGamestatus,"TURN")==0){
        //My Turn
        fnPlace();
    }
    else if(strcmp(chGamestatus,"PLACE")==0){
        //Enemy's Turn
        int iRow,iCol;
        scanf("%d %d",&iRow,&iCol);
        fnFlipTrue(iRow,iCol,iEneside);
        iBoardAll++;
    }
    else if(strcmp(chGamestatus,"START")==0){
        //Game Start, Reply "OK"
        scanf("%d",&iMyside);
        if(iMyside==BLACK){
            iEneside=WHITE;
        }
        else if(iMyside==WHITE){
            iEneside=BLACK;
        }
        printf("OK\n");
        fflush(stdout);
    }
}

//Initialize History before Game Start
//Changed:iHistory
void fnInitHistory(){
    int iSideCount,iStepCount,iPossCount,iRow,iCol;
    for(iSideCount=0;iSideCount<=1;iSideCount++){
        for(iStepCount=0;iStepCount<=SPAN*SPAN-1;iStepCount++){
            for(iPossCount=0;iPossCount<=SPAN*SPAN-1;iPossCount++){
                iRow=(iPossCount+1)/16;
                iCol=iPossCount-iRow*SPAN;
                if(iCol==-1){
                    iCol=15;
                    iRow=iRow-1;
                }
                iHistory[iSideCount][iStepCount][iPossCount]=iRow*1000+iCol;
            }
        }
    }
}

//Before Game Start
//Changed:iBoard,iDiv
void fnInitGame(){
    memset(iBoard,0,sizeof(iBoard));
    iBoard[SPAN/2-1][SPAN/2-1]=iBoard[SPAN/2][SPAN/2]=WHITE;
    iBoard[SPAN/2-1][SPAN/2]=iBoard[SPAN/2][SPAN/2-1]=BLACK;
    int iRC,iCC;
    for(iRC=0;iRC<=SPAN-1;iRC++){
        for(iCC=0;iCC<=SPAN-1;iCC++){
            iDiv[iRC][iCC]=-1;
        }
    }
    iDiv[SPAN/2-1][SPAN/2-1]=iDiv[SPAN/2][SPAN/2]=iDiv[SPAN/2-1][SPAN/2]=iDiv[SPAN/2][SPAN/2-1]=5;
}

int main(int argc, const char * argv[]) {
    fnInitGame();
    
    fnInitHistory();
    
    //Game Start
    while(strcmp(chGamestatus,"END")!=0){
        scanf("%s",chGamestatus);
        fnReply();
    }
}
