#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "Gomoku_Rule.h"

#define INF 149122176
#define MSEARCH 6
#define MKILL 6
#define CONKILL 3
///ɱ���������
#define ONE 1
#define BLOCK2 2
#define FLEX2 3
#define BLOCK3 4
#define DB_BLOCK3 5
#define DB_FLEX2 6
#define FLEX3 7
#define BLOCK4 8
#define FLEX3_BLOCK3 9
#define FLEX3_FLEX2 10
#define FLEX3_BLOCK4 11
#define FLEX4 12
#define FIVE_ 13
#define CHESSTYPE 13
#define TAICHI 111
#define RANGE 50
#define KILL_POINT 3061
#define CPTKILL 1
#define HUMKILL -1

using namespace std;

struct OneStep
{
    int Row, Column;
    int Value;
    OneStep(int r, int c, int v): Row(r), Column(c), Value(v) {}
    OneStep(): Row(0), Column(0), Value(0) {}
    bool operator < (const OneStep & M) const
    {
        return Value > M.Value;
    }
};

const int depConsider[] = {0, 7, 7, 8, 8, 9, 10, 10, 11};

class Gomoku_AI : public Gomoku_Rule
{
private:
    int AIvalue, HMvalue;///��ֵ����
    int depthR[MSEARCH+1], depthC[MSEARCH+1];///������
    int depthkillR[MKILL+1], depthkillC[MKILL+1];///ɱ�в�
    int recentR, recentC;///��һ���ߵ�R��C
    int CopyChessBoard[20][20];///ģ��Ԥ��
    int CPT;
    int KillMost;
    double facHUM;
    int MaxSearchDep;
    int killScores[CHESSTYPE+1];
    //int depConsider[MSEARCH+1];
    ///bool ValidityBlock[20][20];///��������
    bool ValidityBlock[20][20];///��ɨ������
    OneStep MightMove[MSEARCH+1][230];///���ܵ��ƶ�
    OneStep MightkillMove[MKILL+1][226];///���ܵ�ɱ��
    OneStep BestMove;///����߷�
    vector<pair<int, int>> ChangeValidity[MSEARCH+1];///��ɨ�������иı�ļ�¼
    vector<pair<int, int>> ChangekillValidity[MKILL+1];///��ɨ��ɱ���иı�ļ�¼
    int totMight[MSEARCH+1];///���ɲ�
    int totMightkill[MKILL+1];///����ɱ��
public:
    Gomoku_AI(): AIvalue(0), HMvalue(0), CPT(WHITE)
    {
        srand(time(NULL));///��������Գ�ʼ��
        memset(ValidityBlock, 0, sizeof(ValidityBlock));
        for(register int i = 1; i <= Size; i += 1)
            for(register int j = 1; j <= Size; j += 1)
                CopyChessBoard[i][j] = EMPTY;
        facHUM = 1.5;
        for(register int i = 0; i <= CHESSTYPE; i += 1)
            killScores[i] = 0;
        killScores[FIVE_] = INF;
        killScores[FLEX4] = INF-2;
        killScores[BLOCK4] = 225000;
        /*depConsider[KILLDEP+1] = 7;
        depConsider[KILLDEP+2] = 7;
        depConsider[KILLDEP+3] = 8;
        depConsider[KILLDEP+4] = 8;
        depConsider[KILLDEP+5] = 8;
        depConsider[KILLDEP+6] = 10;
        depConsider[KILLDEP+7] = 13;
        depConsider[KILLDEP+8] = 15;*/
    }
    int AlphaBeta(int depth, int alpha, int beta);///�����㷨��AB��֦
    inline void GenerateLegalMoves(int &depth);///�������ܵ��߷�
    inline int NowBlockEvaluate(int r, int c, int wh);///��ǰ��ļ�ֵ
    inline int NowBlockType(int r, int c, int wh, int dir);///��ǰ��ĸ���������
    inline int EvaluateLine(int a[10], int totLen, int totCnt);///���м������ж�
    inline int EvaluateSquare(int r, int c, int wh);///���������ͼ��
    void LetCPTBe(int wh);///���Ծ���
    inline int EvaluateBoard();///��ǰ������������
    void RefreshBoard();///��������
    void PrintCopyBoard();///��ӡ�ڲ�����
    OneStep getBestMove();///����������
    inline void MakeNextMove(OneStep xy, int &depth);///���ܵ���һ���߷�
    inline void UnMakeMove(OneStep xy, int &depth);///�˻�һ��
    inline int AbsoluteInt(int &x) {return x >= 0 ? x : -x;}
    void PrintValidityBlock();
    inline bool CheckCopyValidity(int r, int c);///�������̵ĺ�����
    inline bool CheckCopyWinner(int r, int c);///��鿽������ʤ����
    int getMaxSearchDepth();///�������������
    void Refreshfac();///����������������
    void Changeval(int val);///��ǰAI�÷ָ���
    int killStep(int depth);///ɱ�г���
    inline void GeneratekillMoves(int &depth);///ɱ������
    inline void MakeNextkillMove(OneStep xy, int &depth);///���ܵ���һ��ɱ��
    inline void UnMakekillMove(OneStep xy, int &depth);///�˻�һɱ��
};

void Gomoku_AI::Refreshfac()
{
    if(AIvalue >= 0)
        facHUM = 1.3;
    else if(AIvalue >= -1000)
        facHUM = 1.5;
    else if(AIvalue >= -3050)
        facHUM = 2.0;
    else
        facHUM = 3.0;
}

void Gomoku_AI::Changeval(int val)
{
    AIvalue = val;
}

int Gomoku_AI::getMaxSearchDepth()
{
    return MaxSearchDep;
}

OneStep Gomoku_AI::getBestMove()
{
    return BestMove;
}

void Gomoku_AI::LetCPTBe(int wh)
{
    CPT = wh;
}

void Gomoku_AI::PrintValidityBlock()
{
    printf("  ");
    printf("�ŢƢǢȢɢʢˢ̢͢΢ϢТѢҢ�\n");
    for(register int i = 1; i <= Size; i += 1)
    {
        //printf("%2d", i);
        cout << setw(2) << i;
        for(register int j = 1; j <= Size; j += 1)
            switch(ValidityBlock[i][j])
            {
            case true:
                printf("��");
                break;
            case false:
                printf("��");
                break;
            }
        puts("");
    }
}

void Gomoku_AI::PrintCopyBoard()
{
    printf("  ");
    printf("�ŢƢǢȢɢʢˢ̢͢΢ϢТѢҢ�\n");
    for(register int i = 1; i <= Size; i += 1)
    {
        //printf("%2d", i);
        cout << setw(2) << i;
        for(register int j = 1; j <= Size; j += 1)
            switch(CopyChessBoard[i][j])
            {
            case BLACK:
                printf("��");
                break;
            case WHITE:
                printf("��");
                break;
            case EMPTY:
                printf("��");
                break;
            }
        puts("");
    }
}

void Gomoku_AI::RefreshBoard()
{
    int r, c;
    int rc = GetLastMove();
    r = rc / 100;
    c = rc % 100;
    bool wh = 1^GetNowPlayer();
    CopyChessBoard[r][c] = wh;
    if(ValidityBlock[r][c])
    {
        for(register int i = -2; i <= 2; i += 1)
            for(register int j = -2; j <= 2; j += 1)
                if(CheckInboard(r+i, c+j) && !ValidityBlock[r+i][c+j])
                    ValidityBlock[r+i][c+j] = true;
    }
    else
        for(register int i = -1; i <= 1; i += 1)
            for(register int j = -1; j <= 1; j += 1)
                if(CheckInboard(r+i, c+j))
                    ValidityBlock[r+i][c+j] = true;
}

inline void Gomoku_AI::UnMakeMove(OneStep xy, int &depth)
{
    CopyChessBoard[xy.Row][xy.Column] = EMPTY;
    int s = ChangeValidity[depth].size();
    int r, c;
    for(register int i = 0; i < s; i += 1)
    {
        r = ChangeValidity[depth][i].first;
        c = ChangeValidity[depth][i].second;
        ValidityBlock[r][c] = false;
    }
    ChangeValidity[depth].clear();
}

inline void Gomoku_AI::MakeNextMove(OneStep xy, int &depth)
{
    int r = xy.Row, c = xy.Column;
    CopyChessBoard[r][c] = CPT^depth&1;
    for(register int i = -2; i <= 2; i += 1)
        for(register int j = -2; j <= 2; j += 1)
            if(CheckInboard(r+i, c+j) && !ValidityBlock[r+i][c+j])
                ChangeValidity[depth].push_back(pair<int, int>(r+i, c+j)),
                ValidityBlock[r+i][c+j] = true;
}

inline int Gomoku_AI::EvaluateSquare(int r, int c, int wh)
{
    int ret = 0;
    if(CheckInboard(r-2, c+1) && CheckInboard(r+1, c+2) && CheckInboard(r-1, c+2) &&
       CopyChessBoard[r-2][c+1] == wh && CopyChessBoard[r+1][c+2] == wh && CopyChessBoard[r-1][c+2] == wh)
        ret = 1;///�J�K�I
    else if(CheckInboard(r+1, c+2) && CheckInboard(r+2, c-1) && CheckInboard(r-1, c-2) &&
       CopyChessBoard[r+1][c+2] == wh && CopyChessBoard[r+2][c-1] == wh && CopyChessBoard[r-1][c-2] == wh)
        ret = 1;///�K�L�I
    else if(CheckInboard(r+1, c+2) && CheckInboard(r+2, c-1) && CheckInboard(r-2, c+1) &&
       CopyChessBoard[r+1][c+2] == wh && CopyChessBoard[r+2][c-1] == wh && CopyChessBoard[r-2][c+1] == wh)
        ret = 1;///�K�L�J
    else if(CheckInboard(r-1, c-2) && CheckInboard(r+2, c-1) && CheckInboard(r-2, c+1) &&
       CopyChessBoard[r-1][c-2] == wh && CopyChessBoard[r+2][c-1] == wh && CopyChessBoard[r-2][c+1] == wh)
        ret = 1;///�I�L�J
    ///������б����
    if(CheckInboard(r-2, c-1) && CheckInboard(r+2, c+1) && CheckInboard(r-1, c+2) &&
       CopyChessBoard[r-2][c-1] == wh && CopyChessBoard[r+2][c+1] == wh && CopyChessBoard[r-1][c+2] == wh)
        ret += 1;///�I�K�J
    else if(CheckInboard(r+1, c-2) && CheckInboard(r-1, c+2) && CheckInboard(r-2, c-1) &&
       CopyChessBoard[r+1][c-2] == wh && CopyChessBoard[r-1][c+2] == wh && CopyChessBoard[r-2][c-1] == wh)
        ret += 1;///�J�L�I
    else if(CheckInboard(r+1, c-2) && CheckInboard(r+2, c+1) && CheckInboard(r-2, c-1) &&
       CopyChessBoard[r+1][c-2] == wh && CopyChessBoard[r+2][c+1] == wh && CopyChessBoard[r-2][c-1] == wh)
        ret += 1;///�L�I�K
    else if(CheckInboard(r-1, c+2) && CheckInboard(r+1, c-2) && CheckInboard(r+2, c+1) &&
       CopyChessBoard[r-1][c+2] == wh && CopyChessBoard[r+1][c-2] == wh && CopyChessBoard[r+2][c+1] == wh)
        ret += 1;///�J�L�K
    ///������б����
    return ret;
}

inline int Gomoku_AI::EvaluateLine(int a[10], int totLen, int totCnt)
{
    int toLeftSpace = 0, toRightSpace = 0;
    for(register int i = 1; i <= totLen && a[i] == EMPTY; i += 1)
        toLeftSpace += 1;
    for(register int i = totLen; i > 0 && a[i] == EMPTY; i -= 1)
        toRightSpace += 1;
    int MidSpace = totLen - toLeftSpace - toRightSpace - totCnt;
    int ret = 0;
    switch(totCnt)
    {
    case 1:
        if(toLeftSpace >= 2 && toRightSpace >= 2)
        {
            //cout << "��1��" << endl;
            ret = ONE;
        }
        break;
    case 2:
        if(!MidSpace && toLeftSpace >= 2 && toRightSpace >= 2 ||
           MidSpace && toLeftSpace >= 1 && toRightSpace >= 1 && toLeftSpace + toRightSpace >= 3)
        {
            //cout << "��2��" << endl;
            ret = FLEX2;
        }
        else if(!MidSpace && (toLeftSpace + toRightSpace >= 3) ||
                MidSpace == 1 && (toLeftSpace + toRightSpace >= 2) ||
                MidSpace == 2 && (toLeftSpace >= 1 || toRightSpace >= 1) ||
                MidSpace == 3)
        {
            //cout << "��2��" << endl;
            ret = BLOCK2;
        }
        break;
    case 3:
        if(!MidSpace && toLeftSpace >= 1 && toRightSpace >= 1 && toLeftSpace + toRightSpace >= 3 ||
           MidSpace == 1 && toLeftSpace >= 1 && toRightSpace >= 1)
        {
            //cout << "��3��" << endl;
            ret = FLEX3;
        }
        else if(!MidSpace && (toLeftSpace >= 2 || toRightSpace >= 2) ||
                toLeftSpace >= 1 && toRightSpace >= 1 ||
                MidSpace == 1 && (toLeftSpace >= 1 || toRightSpace >= 1) ||
                MidSpace == 2)
        {
            //cout << "��3��" << endl;
            ret = BLOCK3;
        }
        else if(MidSpace == 3)
        {
            //cout << "��2��" << endl;
            ret = BLOCK2;
        }
        break;
    case 4:
        if(!MidSpace && toLeftSpace >= 1 && toRightSpace >= 1)
        {
            //cout << "��4��" << endl;
            ret = FLEX4;
        }
        else if(!MidSpace && (toLeftSpace >= 1 || toRightSpace >= 1) ||
                MidSpace == 1)
        {
            //cout << "��4��" << endl;
            ret = BLOCK4;
        }
        else if(MidSpace == 2 ||
                MidSpace == 3 && (a[toLeftSpace+2] == EMPTY || a[totLen-toRightSpace-1] == EMPTY))
        {
            //cout << "��3��" << endl;
            ret = BLOCK3;
        }
        else if(MidSpace == 3)
        {
            //cout << "��2��" << endl;
            ret = BLOCK2;
        }
        break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        if(!MidSpace)
        {
            //cout << "�����飡" << endl;
            ret = FIVE_;
        }
        else if(MidSpace == 1 ||
                MidSpace == 2 && (a[toLeftSpace+2] == EMPTY || a[totLen-toRightSpace-1] == EMPTY))
        {
            //cout << "��4��" << endl;
            ret = BLOCK4;
        }
        break;
    }
    return ret;
}

inline bool Gomoku_AI::CheckCopyValidity(int r, int c)
{
    return CopyChessBoard[r][c] == EMPTY && r > 0 && c > 0 && r <= Size && c <= Size;
}

inline bool Gomoku_AI::CheckCopyWinner(int r, int c)
{
    int checkline[10];
    int tot;
    ///����
    tot = 0;
    for(register int i = c-4; i <= c+4; i += 1)
        if(CheckInboard(r, i))
        {
            checkline[++tot] = CopyChessBoard[r][i];
        }
    ///THIS IS 'DEBUG'
    //for(register int i = 1; i <= tot; i += 1)
    //    cout << checkline[i] ;
    //getchar();
    //getchar();
    if(LineWinning(checkline, tot))
        return true;
    ///����
    tot = 0;
    for(register int i = r-4; i <= r+4; i += 1)
        if(CheckInboard(i, c))
        {
            checkline[++tot] = CopyChessBoard[i][c];
        }
    if(LineWinning(checkline, tot))
        return true;
    ///��������
    tot = 0;
    for(register int i = -4; i <= 4; i += 1)
        if(CheckInboard(r+i, c+i))
        {
            checkline[++tot] = CopyChessBoard[r+i][c+i];
        }
    if(LineWinning(checkline, tot))
        return true;
    ///��������
    tot = 0;
    for(register int i = -4; i <= 4; i += 1)
        if(CheckInboard(r+i, c-i))
        {
            checkline[++tot] = CopyChessBoard[r+i][c-i];
        }
    if(LineWinning(checkline, tot))
        return true;
    return false;
}

inline int Gomoku_AI::NowBlockType(int r, int c, int wh, int dir)
{
    register int i, j;
    int MaxValue = 0, NowValue;
    int LineCheck[10];
    int Cnt, tot;
    int _wh = wh ^ 1;
    //int totVal = 0;
    ///����
    if(dir == 1)
    {
        Cnt = 1;
        for(i = 1; c-i > 0 && CopyChessBoard[r][c-i] != _wh && i <= 4; i += 1)
            if(CopyChessBoard[r][c-i] == wh)
                Cnt += 1;
        i -= 1;
        for(j = 1; c+j <= Size && CopyChessBoard[r][c+j] != _wh && j <= 4; j += 1)
            if(CopyChessBoard[r][c+j] == wh)
                Cnt += 1;
        j -= 1;
        tot = i + j + 1;
        for(register int k = 0; k < tot; k += 1)
            LineCheck[k+1] = CopyChessBoard[r][c-i+k];
        LineCheck[i+1] = wh;///IMPORTANT!!!
        //cout << "Line, " << Cnt << endl;
        return EvaluateLine(LineCheck, tot, Cnt);
    }
    /*if(pl)
        totVal += NowValue;
    if(NowValue > MaxValue)
        MaxValue = NowValue;*/
    ///����
    if(dir == 2)
    {
        Cnt = 1;
        for(i = 1; r-i > 0 && CopyChessBoard[r-i][c] != _wh && i <= 4; i += 1)
            if(CopyChessBoard[r-i][c] == wh)
                Cnt += 1;
        i -= 1;
        for(j = 1; r+j <= Size && CopyChessBoard[r+j][c] != _wh && j <= 4; j += 1)
            if(CopyChessBoard[r+j][c] == wh)
                Cnt += 1;
        j -= 1;
        tot = i + j + 1;
        for(register int k = 0; k < tot; k += 1)
            LineCheck[k+1] = CopyChessBoard[r-i+k][c];
        LineCheck[i+1] = wh;///IMPORTANT!!!
        //cout << "Line, " << Cnt << endl;
        return EvaluateLine(LineCheck, tot, Cnt);
    }

    /*if(pl)
        totVal += NowValue;
    if(NowValue > MaxValue)
        MaxValue = NowValue;*/
    ///��������
    if(dir == 3)
    {
        Cnt = 1;
        for(i = 1; r-i > 0 && c-i > 0 && CopyChessBoard[r-i][c-i] != _wh && i <= 4; i += 1)
            if(CopyChessBoard[r-i][c-i] == wh)
                Cnt += 1;
        i -= 1;
        for(j = 1; r+j <= Size && c+j <= Size && CopyChessBoard[r+j][c+j] != _wh && j <= 4; j += 1)
            if(CopyChessBoard[r+j][c+j] == wh)
                Cnt += 1;
        j -= 1;
        tot = i + j + 1;
        for(register int k = 0; k < tot; k += 1)
            LineCheck[k+1] = CopyChessBoard[r-i+k][c-i+k];
        LineCheck[i+1] = wh;///IMPORTANT!!!
        //cout << "Line, " << Cnt << endl;
        return EvaluateLine(LineCheck, tot, Cnt);
    }
    /*if(pl)
        totVal += NowValue;
    if(NowValue > MaxValue)
        MaxValue = NowValue;*/
    ///��������
    Cnt = 1;
    for(i = 1; r+i <= Size && c-i > 0 && CopyChessBoard[r+i][c-i] != _wh && i <= 4; i += 1)
        if(CopyChessBoard[r+i][c-i] == wh)
            Cnt += 1;
    i -= 1;
    for(j = 1; r-j > 0 && c+j <= Size && CopyChessBoard[r-j][c+j] != _wh && j <= 4; j += 1)
        if(CopyChessBoard[r-j][c+j] == wh)
            Cnt += 1;
    j -= 1;
    tot = i + j + 1;
    for(register int k = 0; k < tot; k += 1)
        LineCheck[k+1] = CopyChessBoard[r+i-k][c-i+k];
    LineCheck[i+1] = wh;///IMPORTANT!!!
    //cout << "Line, " << tot << endl;
    return EvaluateLine(LineCheck, tot, Cnt);
    /*if(pl)
        totVal += NowValue;
    if(NowValue > MaxValue)
        MaxValue = NowValue;*/
    ///��������
    /**totVal = EvaluateSquare(r, c, wh) * TAICHI;
    totVal += rand() % 250;///���¸���*/
    /*if(pl)
        totVal += NowValue;
    if(NowValue > MaxValue)
        MaxValue = NowValue;*/
    //if(pl)
    //    return totVal;
    //return MaxValue;
    ///���·��ú�
    /*if(typeChess[FIVE_])
        totVal = INF;
    else if(typeChess[FLEX4])
        totVal += 225000;
    else if(typeChess[FLEX3] + typeChess[BLOCK4] > 1)
        totVal += 225000;
    else if(typeChess[BLOCK4])
        totVal += 2999;
    else if(typeChess[FLEX3])
        totVal += 3011;
    else if(typeChess[FLEX2] > 1)
        totVal += 555;
    else if(typeChess[BLOCK3])
        totVal += 100;
    else if(typeChess[FLEX2])
        totVal += 103;
    else if(typeChess[BLOCK2])
        totVal += 17;
    else if(typeChess[ONE])
        totVal += 3;
    return totVal;*/
}

inline int Gomoku_AI::NowBlockEvaluate(int r, int c, int wh)
{
    int TypeID = 0, v;
    int totVal = EvaluateSquare(r, c, wh) * TAICHI;
    //totVal += rand() % RANGE;
    int typeChess[CHESSTYPE+1] = {0}, whtype[CHESSTYPE+1] = {0}, _whtype[CHESSTYPE+1] = {0};
    for(register int i = 1; i <= 4; i += 1)
    {
        typeChess[v=NowBlockType(r, c, wh, i)] += 1;
        whtype[v] += 1;
        typeChess[v=NowBlockType(r, c, wh^1, i)] += 1;
        _whtype[v] += 1;
    }
    if(typeChess[FLEX3] + typeChess[BLOCK4] > 1)
        typeChess[FLEX3_BLOCK4] = 1;
    if(typeChess[FLEX2] && typeChess[FLEX3])
        typeChess[FLEX3_FLEX2] = 1;
    if(typeChess[BLOCK3] && typeChess[FLEX3])
        typeChess[FLEX3_BLOCK3] = 1;
    if(typeChess[DB_FLEX2] > 1)
        typeChess[DB_FLEX2] = 1;
    if(typeChess[BLOCK3] > 1)
        typeChess[DB_BLOCK3] = 1;
    for(register int i = FIVE_; i; i -= 1)
        if(typeChess[i])
        {
            TypeID = i;
            break;///kill point
        }
    int whScore = (whtype[BLOCK3]+whtype[FLEX2]) * 10 + (whtype[FLEX3]+whtype[BLOCK4]) * 35;
    int _whScore = (whtype[BLOCK3]+whtype[FLEX2]) * 5 + (whtype[FLEX3]+whtype[BLOCK4]) * 30;
    totVal += whScore + _whScore;
    if(TypeID == FIVE_)
        totVal = INF;
    else if(TypeID == FLEX4)
        totVal += 225000;
    else if(TypeID == FLEX3_BLOCK4)
        totVal += 225000;
    else if(TypeID == FLEX3_FLEX2)
        totVal += 8247;
    else if(TypeID == FLEX3_BLOCK3)
        totVal += 6666;
    else if(TypeID == BLOCK4)
        totVal += 5233;
    else if(TypeID == FLEX3)
        totVal += 3011;
    else if(TypeID == DB_FLEX2)
        totVal += 824;
    else if(TypeID == DB_BLOCK3)
        totVal += 715;
    else if(TypeID == BLOCK3)
        totVal += 100;
    else if(TypeID == FLEX2)
        totVal += 103;
    //else if(TypeID == BLOCK2)
    //    totVal += 17;
    //else if(TypeID == ONE)
    //    totVal += 3;
    return totVal;
}
///���ӱ���Ҫ���ܼ���������������
inline void Gomoku_AI::GenerateLegalMoves(int &depth)
{
    totMight[depth] = 0;
    for(register int i = 1; i <= Size; i += 1)
        for(register int j = 1; j <= Size; j += 1)
            if(ValidityBlock[i][j] && CopyChessBoard[i][j] == EMPTY)
                MightMove[depth][++totMight[depth]] = OneStep(i, j, NowBlockEvaluate(i, j, CPT^depth&1));
    ///sort to find better move
    sort(MightMove[depth]+1, MightMove[depth]+totMight[depth]+1);
    /*if(depth == MSEARCH)
        for(register int i = 1; i <= totMight[depth]; i += 1)
            cout << MightMove[depth][i].Value << endl;*/
    /*if(MightMove[depth][1].Value == INF)
    cout << MightMove[depth][1].Value << ' ' << MightMove[depth][2].Value << endl,
    getchar(),
    getchar();*/
}

inline int Gomoku_AI::EvaluateBoard()
{
    int CPTType[CHESSTYPE+1] = {0}, HUMType[CHESSTYPE+1] = {0};
    int CPTEight = 0, HUMEight = 0;
    int eval = 0, range_eval = 0;///����Ҫ���¸���
    for(register int i = 1; i <= Size; i += 1)
        for(register int j = 1; j <= Size; j += 1)
            if(CopyChessBoard[i][j] == CPT)
            {
                for(register int k = 1; k <= 4; k += 1)
                    CPTType[NowBlockType(i, j, CPT, k)] += 1;
                CPTEight += EvaluateSquare(i, j, CPT);
            }
            else if(CopyChessBoard[i][j] == (CPT^1))
            {
                for(register int k = 1; k <= 4; k += 1)
                    HUMType[NowBlockType(i, j, CPT^1, k)] += 1;
                HUMEight += EvaluateSquare(i, j, CPT^1);
            }
    if(CPTType[BLOCK4] || CPTType[FLEX4] || CPTType[FLEX3_BLOCK4])///��ɱ��
        return INF-2;///��ֹ��ȥ��5
    if(HUMType[FLEX4] || HUMType[FLEX3_BLOCK4])
        return -INF+2;
    if(CPTType[FLEX3_FLEX2] || CPTType[FLEX3] || CPTType[FLEX3_BLOCK3])
        return 225000;
    if(HUMType[BLOCK4])
        eval = -184514;
    eval -= 4122 * HUMType[FLEX3]/3 + 7122 * HUMType[FLEX3_BLOCK3];
    eval += CPTType[DB_FLEX2] * 8247 - HUMType[DB_FLEX2] * 2333;
    eval += CPTType[DB_BLOCK3] * 6666 - HUMType[DB_BLOCK3] * 1234;
    eval += (CPTType[BLOCK3] * 5233 - HUMType[BLOCK3] * 999) / 3;
    eval += CPTType[FLEX2] * 3011 - HUMType[FLEX2] * 233 >> 1;
    eval += CPTType[BLOCK2] * 100 - HUMType[BLOCK2] * 28 >> 1;
    eval += CPTType[ONE] * 103 - HUMType[ONE] * 10;
    eval += CPTEight * 111 - HUMEight * 99;
    //range_eval = rand() % (RANGE<<1|1) - RANGE;///��������
    ///�ķ�����
    return eval;
}

inline void Gomoku_AI::UnMakekillMove(OneStep xy, int &depth)
{
    CopyChessBoard[xy.Row][xy.Column] = EMPTY;
    int s = ChangekillValidity[depth].size();
    int r, c;
    for(register int i = 0; i < s; i += 1)
    {
        r = ChangekillValidity[depth][i].first;
        c = ChangekillValidity[depth][i].second;
        ValidityBlock[r][c] = false;
    }
    ChangekillValidity[depth].clear();
}

inline void Gomoku_AI::MakeNextkillMove(OneStep xy, int &depth)
{
    int r = xy.Row, c = xy.Column;
    CopyChessBoard[r][c] = CPT^depth&1;
    for(register int i = -2; i <= 2; i += 1)
        for(register int j = -2; j <= 2; j += 1)
            if(CheckInboard(r+i, c+j) && !ValidityBlock[r+i][c+j])
                ChangekillValidity[depth].push_back(pair<int, int>(r+i, c+j)),
                ValidityBlock[r+i][c+j] = true;
}

inline void Gomoku_AI::GeneratekillMoves(int &depth)
{
    totMightkill[depth] = 0;
    int getKill, bestScore = 0;
    for(register int i = 1; i <= Size; i += 1)
        for(register int j = 1; j <= Size; j += 1)
            if(ValidityBlock[i][j] && CopyChessBoard[i][j] == EMPTY)
            {
                ///ɱ��
                bestScore = 0;
                for(register int k = 1; k <= 4; k += 1)
                {
                    getKill = NowBlockType(i, j, CPT^depth&1, k);
                    if(bestScore < killScores[getKill])
                        bestScore = killScores[getKill];
                }
                for(register int k = 1; k <= 4; k += 1)
                {
                    getKill = NowBlockType(i, j, 1^CPT^depth&1, k);
                    if(bestScore < killScores[getKill])
                        bestScore = killScores[getKill];
                }
                if(bestScore)
                    MightkillMove[depth][++totMightkill[depth]] = OneStep(i, j, bestScore);
            }
    ///sort to find better move
    sort(MightkillMove[depth]+1, MightkillMove[depth]+totMightkill[depth]+1);
    /*if(depth == MSEARCH)
        for(register int i = 1; i <= totMight[depth]; i += 1)
            cout << MightMove[depth][i].Value << endl;*/
    /*if(MightMove[depth][1].Value == INF)
    cout << MightMove[depth][1].Value << ' ' << MightMove[depth][2].Value << endl,
    getchar(),
    getchar();*/
}

int Gomoku_AI::killStep(int depth)///ɱ��
{
    ///û��ɱ����0
    if(depth < KillMost && CheckCopyWinner(depthkillR[depth+1], depthkillC[depth+1]))
        return (depth&1) ? CPTKILL : HUMKILL;///����һ��ɱ���淵���ԣ�ż������
    if(!depth)
        return 0;
    GeneratekillMoves(depth);
    if(!totMightkill[depth])
        return 0;
    int maxCon = min(CONKILL, totMightkill[depth]);
    MakeNextkillMove(MightkillMove[depth][1], depth);
    depthkillR[depth] = MightkillMove[depth][1].Row;
    depthkillC[depth] = MightkillMove[depth][1].Column;
    int ret = killStep(depth-1), nw;
    UnMakekillMove(MightkillMove[depth][1], depth);
    if((depth&1) && ret == HUMKILL)
        return HUMKILL;
    if(!(depth&1) && ret == CPTKILL)
        return CPTKILL;
    for(register int i = 2; i <= maxCon; i += 1)
    {
        MakeNextkillMove(MightkillMove[depth][i], depth);
        depthkillR[depth] = MightkillMove[depth][i].Row;
        depthkillC[depth] = MightkillMove[depth][i].Column;
        nw = killStep(depth-1);
        UnMakekillMove(MightkillMove[depth][i], depth);
        if(depth & 1)///���࿼�ǣ���С����
        {
            if(nw == HUMKILL)///�ض�����
                return HUMKILL;
            if(nw < ret)
                ret = nw;
        }
        else///���Կ��ǣ������ȥ
        {
            if(nw == CPTKILL)///�ض�����
                return CPTKILL;
            if(nw > ret)
                ret = nw;
        }
    }
    return ret;
}

int Gomoku_AI::AlphaBeta(int depth, int alpha, int beta)
{
    int val = alpha, maxCon;
    //getchar();
    //cout << "Depth : " << depth;
    /*if(depth == MSEARCH)
        MaxSearchDep = 1;
    MaxSearchDep = MSEARCH-depth+1 > MaxSearchDep ? MSEARCH-depth+1 : MaxSearchDep;*/
    if(depth < MSEARCH && CheckCopyWinner(depthR[depth+1], depthC[depth+1]))
        return -INF;
    if(!depth)
    {
        /*KillMost = MKILL;
        int killer = killStep(MKILL);///��ɱ
        if(killer == CPTKILL)
            return INF;
        else if(killer == HUMKILL)
            return -INF;*/
        return EvaluateBoard();
    }
    ///��¼�������
    GenerateLegalMoves(depth);
    if(depth == MSEARCH)
        BestMove = MightMove[depth][1];
    if(depth < MSEARCH && totMight[depth] >= 2 && MightMove[depth][1].Value >= KILL_POINT && MightMove[depth][2].Value >= KILL_POINT)///������ɱ
    {
        if(depth&1)
        {
            KillMost = MKILL-1;
            int killer = killStep(MKILL-1);///��ɱ
            if(killer == HUMKILL)
                return INF;
        }
        else
        {
            KillMost = MKILL;
            int killer = killStep(MKILL);///��ɱ
            if(killer == CPTKILL)
                return INF;
        }
    }
    maxCon = min(totMight[depth], depConsider[depth]);
    ///���ǳ������
    /*if(depth == MSEARCH)
        printf("%d\n", maxCon), getchar(), getchar();*/
    for(register int i = 1; i <= maxCon; i += 1)
    {
        MakeNextMove(MightMove[depth][i], depth);
        depthR[depth] = MightMove[depth][i].Row;
        depthC[depth] = MightMove[depth][i].Column;
        //PrintCopyBoard();
        val = -AlphaBeta(depth-1, -beta, -alpha);
        //printf("%d\n", val);
        //getchar();
        UnMakeMove(MightMove[depth][i], depth);
        if(val >= beta)
        {
            if(depth == MSEARCH)
                BestMove = MightMove[depth][i];
            return beta;
        }
        if(val > alpha)
        {
            if(depth == MSEARCH)
                BestMove = MightMove[depth][i];
            alpha = val;
        }
    }
    return alpha;
}

///ģ��һ�������HUM��һ��ɱ��

/**

depth = 10, alpha = -INF, beta = INF

depth = 9, alpha = -beta = -INF, beta = -alpha = INF

depth = 8, ����ɱ��, return -INF

depth = 9, val = AB(8) = INF >= beta(INF), return INF

depth = 10, val = -INF������´𰸣����ǵ�ס���Ǹ���

but, we find some way to stop that, val > -INF

���ˣ������þ�������������

*/
