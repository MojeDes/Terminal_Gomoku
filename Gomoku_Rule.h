#include <iostream>
#include <cstring>
#include <Windows.h>
#include <iomanip>

#define RETURN_ERROR return -1
#define BLACK 0
#define WHITE 1
#define EMPTY 2
#define Size 15

using namespace std;

/*******************规则部分******************/
class Gomoku_Rule
{
private:
    int ChessBoard[20][20];///棋盘不应被其他人访问
    int WBTurn;///黑白走子
    int Steps;
    int RowRecord[300], ColumnRecord[300];///行列走子记录
public:
    Gomoku_Rule(): WBTurn(false), Steps(1)
    {
        memset(RowRecord, 0, sizeof(RowRecord));
        memset(ColumnRecord, 0, sizeof(ColumnRecord));
        for(register int i = 1; i <= Size; i += 1)
            for(register int j = 1; j <= Size; j += 1)
                ChessBoard[i][j] = EMPTY;///空气盘是用2填充的
    }
    bool CheckValidity(int r, int c);///检查落子合理性
    bool CheckInboard(int r, int c);///在棋盘内
    bool LineWinning(int a[], int tot);///辅助检查
    bool CheckWinner(int r, int c);///检查胜负接口
    int PutChess(int r, int c);///放置棋子接口
    int PutChessBeforeGame(int r, int c, int wh);///摆谱接口
    int BackMove();///悔棋接口
    void Print_Board();///打印棋盘
    int GetLastMove();///AI接口，获得对手的移动
    ///int* Return_Board();///输出棋盘接口
    int GetNowPlayer();///获得当前玩家
    void Print_Manual();///打印棋谱
    ///getsize
    ///*打印棋谱
    ///*让步操作（A直接切换玩家，B中间空一步）
};

void Gomoku_Rule::Print_Manual()
{
    printf("棋谱如下(行,列)：\n");
    for(register int i = 1; i < Steps; i += 2)
        printf("●(%2d, %2d)\t○(%2d, %2d)\n", RowRecord[i], ColumnRecord[i], RowRecord[i+1], ColumnRecord[i+1]);
}

int Gomoku_Rule::GetLastMove()
{
    return RowRecord[Steps-1] * 100 + ColumnRecord[Steps-1];
}

int Gomoku_Rule::PutChessBeforeGame(int r, int c, int wh)
{
    if(CheckInboard(r, c) && wh >= 0 && wh <= 2)
    {
        ChessBoard[r][c] = wh;
        return 0;
    }
    RETURN_ERROR;
}

int Gomoku_Rule::GetNowPlayer()
{
    return WBTurn;
}

bool Gomoku_Rule::CheckValidity(int r, int c)
{
    return ChessBoard[r][c] == EMPTY && r > 0 && c > 0 && r <= Size && c <= Size;
}

bool Gomoku_Rule::CheckInboard(int r, int c)
{
    return r > 0 && c > 0 && r <= Size && c <= Size;
}

bool Gomoku_Rule::LineWinning(int a[], int tot)
{
    int wh = a[1];
    int suc_chess = 1;
    for(register int i = 2; i <= tot; i += 1)
    {
        if(wh != a[i])
            wh = a[i], suc_chess = 1;///不连续则重置
        else if(wh != EMPTY)
            suc_chess += 1;///连续则增加
        if(suc_chess == 5)
            return true;///胜利者出现了
    }
    return false;///胜利者未出现
}

bool Gomoku_Rule::CheckWinner(int r, int c)
{
    int checkline[10];
    int tot;
    ///左右
    tot = 0;
    for(register int i = c-4; i <= c+4; i += 1)
        if(CheckInboard(r, i))
        {
            checkline[++tot] = ChessBoard[r][i];
        }
    ///THIS IS 'DEBUG'
    //for(register int i = 1; i <= tot; i += 1)
    //    cout << checkline[i] ;
    //getchar();
    //getchar();
    if(LineWinning(checkline, tot))
        return true;
    ///上下
    tot = 0;
    for(register int i = r-4; i <= r+4; i += 1)
        if(CheckInboard(i, c))
        {
            checkline[++tot] = ChessBoard[i][c];
        }
    if(LineWinning(checkline, tot))
        return true;
    ///左上右下
    tot = 0;
    for(register int i = -4; i <= 4; i += 1)
        if(CheckInboard(r+i, c+i))
        {
            checkline[++tot] = ChessBoard[r+i][c+i];
        }
    if(LineWinning(checkline, tot))
        return true;
    ///右上左下
    tot = 0;
    for(register int i = -4; i <= 4; i += 1)
        if(CheckInboard(r+i, c-i))
        {
            checkline[++tot] = ChessBoard[r+i][c-i];
        }
    if(LineWinning(checkline, tot))
        return true;
    return false;
}

int Gomoku_Rule::PutChess(int r, int c)
{
    if(CheckValidity(r, c))
    {
        ChessBoard[r][c] = WBTurn;
        RowRecord[Steps] = r;
        ColumnRecord[Steps] = c;///记录落子
        Steps += 1;
        WBTurn ^= 1;
        if(CheckWinner(r, c))
            return 1;///发现赢家，返回1信号
        return 0;///常规返回值
    }
    RETURN_ERROR;///异常操作
}

int Gomoku_Rule::BackMove()
{
    if(Steps > 1)
    {
        ChessBoard[RowRecord[Steps]][ColumnRecord[Steps]] = EMPTY;///清空
        Steps--;
        WBTurn ^= 1;///谁下的反过来
        return 0;
    }
    RETURN_ERROR;///异常操作
}

void Gomoku_Rule::Print_Board()
{
    printf("  ");
    printf("⑴  ⑵  ⑶  ⑷  ⑸  ⑹  ⑺  ⑻  ⑼  ⑽  ⑾  ⑿  ⒀  ⒁  ⒂\n");
    printf(" 1");
    switch(ChessBoard[1][1])
    {
    case BLACK:
        printf("●");
        break;
    case WHITE:
        printf("○");
        break;
    case EMPTY:
        printf("┏");
        break;
    }
    printf("━");
    for(register int j = 2; j < Size; j += 1)
    {
        switch(ChessBoard[1][j])
        {
        case BLACK:
            printf("●");
            break;
        case WHITE:
            printf("○");
            break;
        case EMPTY:
            printf("┳");
            break;
        }
        printf("━");
    }
    switch(ChessBoard[1][Size])
    {
    case BLACK:
        printf("●");
        break;
    case WHITE:
        printf("○");
        break;
    case EMPTY:
        printf("┓");
        break;
    }
    printf("\n");
    printf("  ");
    for(register int j = 1; j <= Size; j += 1)
        printf("┃  ");
    printf("\n");
    for(register int i = 2; i < Size; i += 1)
    {
        //printf("%2d", i);
        cout << setw(2) << i;
        switch(ChessBoard[i][1])
        {
        case BLACK:
            printf("●");
            break;
        case WHITE:
            printf("○");
            break;
        case EMPTY:
            printf("┣");
            break;
        }
        printf("━");
        for(register int j = 2; j < Size; j += 1)
        {
            switch(ChessBoard[i][j])
            {
            case BLACK:
                printf("●");
                break;
            case WHITE:
                printf("○");
                break;
            case EMPTY:
                printf("╋");
                break;
            }
            printf("━");
        }
        switch(ChessBoard[i][Size])
        {
        case BLACK:
            printf("●");
            break;
        case WHITE:
            printf("○");
            break;
        case EMPTY:
            printf("┫");
            break;
        }
        puts("");
        printf("  ");
        for(register int j = 1; j <= Size; j += 1)
            printf("┃  ");
        puts("");
    }
    printf("15");
    switch(ChessBoard[Size][1])
    {
    case BLACK:
        printf("●");
        break;
    case WHITE:
        printf("○");
        break;
    case EMPTY:
        printf("┗");
        break;
    }
    printf("━");
    for(register int j = 2; j < Size; j += 1)
    {
        switch(ChessBoard[Size][j])
        {
        case BLACK:
            printf("●");
            break;
        case WHITE:
            printf("○");
            break;
        case EMPTY:
            printf("┻");
            break;
        }
        printf("━");
    }
    switch(ChessBoard[Size][Size])
    {
    case BLACK:
        printf("●");
        break;
    case WHITE:
        printf("○");
        break;
    case EMPTY:
        printf("┛");
        break;
    }
    puts("");
}
/*******************规则部分******************/
