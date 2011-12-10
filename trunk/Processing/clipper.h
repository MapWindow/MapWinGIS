/*******************************************************************************
*                                                                              *
* Author    :  Angus Johnson                                                   *
* Version   :  3.1.0                                                           *
* Date      :  17 February 2011                                                *
* Website   :  http://www.angusj.com                                           *
* Copyright :  Angus Johnson 2010-2011                                         *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
* Attributions:                                                                *
* The code in this library is an extension of Bala Vatti's clipping algorithm: *
* "A generic solution to polygon clipping"                                     *
* Communications of the ACM, Vol 35, Issue 7 (July 1992) pp 56-63.             *
* http://portal.acm.org/citation.cfm?id=129906                                 *
*                                                                              *
* Computer graphics and geometric modeling: implementation and algorithms      *
* By Max K. Agoston                                                            *
* Springer; 1 edition (January 4, 2005)                                        *
* Pages 98 - 106.                                                              *
* http://books.google.com/books?q=vatti+clipping+agoston                       *
*                                                                              *
*******************************************************************************/

/*******************************************************************************
*                                                                              *
* This is a translation of the Delphi Clipper library and the naming style     *
* used has retained a very strong Delphi flavour.                              *
*                                                                              *
*******************************************************************************/

#ifndef clipper_hpp
#define clipper_hpp

#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

namespace clipper {

enum TClipType { ctIntersection, ctUnion, ctDifference, ctXor };
enum TPolyType { ptSubject, ptClip };
enum TPolyFillType { pftEvenOdd, pftNonZero};

struct TDoublePoint { double X; double Y; };
struct TDoubleRect { double left; double top; double right; double bottom; };
typedef std::vector< TDoublePoint > TPolygon;
typedef std::vector< TPolygon > TPolyPolygon;

TDoublePoint DoublePoint(const double &X, const double &Y);
TPolyPolygon OffsetPolygons(const TPolyPolygon &pts, const double &delta);
TDoubleRect GetBounds(const TPolygon &poly);
bool IsClockwise(const TPolygon &poly);
TDoublePoint GetUnitNormal( const TDoublePoint &pt1, const TDoublePoint &pt2);

//used internally ...
enum TEdgeSide { esLeft, esRight };
enum TIntersectProtects { ipNone = 0, ipLeft = 1, ipRight = 2, ipBoth = 3 };

struct TEdge {
  double xbot;
  double ybot;
  double xcurr;
  double ycurr;
  double xtop;
  double ytop;
  double dx;
  double tmpX;
  bool nextAtTop;
  TPolyType polyType;
  TEdgeSide side;
  int windDelta; //1 or -1 depending on winding direction
  int windCnt;
  int windCnt2; //winding count of the opposite polytype
  int outIdx;
  TEdge *next;
  TEdge *prev;
  TEdge *nextInLML;
  TEdge *nextInAEL;
  TEdge *prevInAEL;
  TEdge *nextInSEL;
  TEdge *prevInSEL;
};

struct TIntersectNode {
  TEdge *edge1;
  TEdge *edge2;
  TDoublePoint pt;
  TIntersectNode *next;
  TIntersectNode *prev;
};

struct TLocalMinima {
  double Y;
  TEdge *leftBound;
  TEdge *rightBound;
  TLocalMinima *nextLm;
};

struct TScanbeam {
  double Y;
  TScanbeam *nextSb;
};

//nb: ofClockwise = desired orientation flag; ofCW = current orientation flag
enum TOrientationFlag {ofEmpty = 0, ofClockwise = 1, ofCW = 2,
  ofForwardBound = 4, ofTop = 8, ofBottomMinima = 16};

inline TOrientationFlag operator|(TOrientationFlag a, TOrientationFlag b)
{return static_cast<TOrientationFlag>(static_cast<int>(a) | static_cast<int>(b));}

struct TPolyPt {
  TDoublePoint pt;
  TPolyPt *next;
  TPolyPt *prev;
  TOrientationFlag flags;
  double dx;
};

struct TJoinRec {
    TDoublePoint pt;
    int idx1;
    int idx2;
    TPolyPt* outPPt; //used by horizontal joins only
};

typedef std::vector < TPolyPt * > PolyPtList;
typedef std::vector < TJoinRec > JoinList;
typedef std::vector< TEdge* > EdgeList;

//ClipperBase is the ancestor to the Clipper class. It should not be
//instantiated directly. This class simply abstracts the conversion of sets of
//polygon coordinates into edge objects that are stored in a LocalMinima list.
class ClipperBase
{
public:
  ClipperBase();
  virtual ~ClipperBase();
  void AddPolygon(const TPolygon &pg, TPolyType polyType);
  void AddPolyPolygon( const TPolyPolygon &ppg, TPolyType polyType);
  virtual void Clear();
  TDoubleRect GetBounds();
protected:
  void DisposeLocalMinimaList();
  void InsertLocalMinima(TLocalMinima *newLm);
  TEdge* AddBoundsToLML(TEdge *e);
  void PopLocalMinima();
  bool Reset();
  TLocalMinima           *m_CurrentLM;
private:
  TLocalMinima           *m_localMinimaList;
  EdgeList               m_edges;
};

class Clipper : public virtual ClipperBase
{
public:
  Clipper();
  ~Clipper();
  bool Execute(TClipType clipType,
  TPolyPolygon &solution,
  TPolyFillType subjFillType = pftEvenOdd,
  TPolyFillType clipFillType = pftEvenOdd);
  //IgnoreOrientation: the Execute method will be approx 60% faster if
  //Clipper doesn't need to calculate the orientation of output polygons ...
  bool IgnoreOrientation();
  void IgnoreOrientation(bool value);
private:
  PolyPtList        m_PolyPts;
  JoinList          m_Joins;
  JoinList          m_CurrentHorizontals;
  TClipType         m_ClipType;
  TScanbeam        *m_Scanbeam;
  TEdge            *m_ActiveEdges;
  TEdge            *m_SortedEdges;
  TIntersectNode   *m_IntersectNodes;
  bool              m_ExecuteLocked;
  bool              m_IgnoreOrientation;
  TPolyFillType     m_ClipFillType;
  TPolyFillType     m_SubjFillType;
  double            m_IntersectTolerance;
  void UpdateHoleStates();
  void DisposeScanbeamList();
  void SetWindingDelta(TEdge& edge);
  void SetWindingCount(TEdge& edge);
  bool IsNonZeroFillType(const TEdge& edge) const;
  bool IsNonZeroAltFillType(const TEdge& edge) const;
  bool InitializeScanbeam();
  void InsertScanbeam(const double &Y);
  double PopScanbeam();
  void InsertLocalMinimaIntoAEL(const double &botY);
  void InsertEdgeIntoAEL(TEdge *edge);
  void AddEdgeToSEL(TEdge *edge);
  void CopyAELToSEL();
  void DeleteFromSEL(TEdge *e);
  void DeleteFromAEL(TEdge *e);
  void UpdateEdgeIntoAEL(TEdge *&e);
  void SwapPositionsInSEL(TEdge *edge1, TEdge *edge2);
  bool Process1Before2(TIntersectNode *Node1, TIntersectNode *Node2);
  bool TestIntersections();
  bool IsContributing(const TEdge& edge) const;
  bool IsTopHorz(const double &XPos);
  void SwapPositionsInAEL(TEdge *edge1, TEdge *edge2);
  void DoMaxima(TEdge *e, const double &topY);
  void ProcessHorizontals();
  void ProcessHorizontal(TEdge *horzEdge);
  void AddLocalMaxPoly(TEdge *e1, TEdge *e2, const TDoublePoint &pt);
  void AddLocalMinPoly(TEdge *e1, TEdge *e2, const TDoublePoint &pt);
  void AppendPolygon(TEdge *e1, TEdge *e2);
  void DoEdge1(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt);
  void DoEdge2(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt);
  void DoBothEdges(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt);
  void IntersectEdges(TEdge *e1, TEdge *e2,
     const TDoublePoint &pt, TIntersectProtects protects);
  TPolyPt* AddPolyPt(TEdge *e, const TDoublePoint &pt);
  TPolyPt* InsertPolyPtBetween(const TDoublePoint &pt, TPolyPt* pp1, TPolyPt* pp2);
  void DisposeAllPolyPts();
  void ProcessIntersections( const double &topY);
  void AddIntersectNode(TEdge *e1, TEdge *e2, const TDoublePoint &pt);
  void BuildIntersectList(const double& topY);
  void ProcessIntersectList();
  TEdge* BubbleSwap(TEdge *edge);
  void ProcessEdgesAtTopOfScanbeam(const double &topY);
  void BuildResult(TPolyPolygon& polypoly);
  void DisposeIntersectNodes();
  void MergePolysWithCommonEdges();
  void FixOrientation();
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class clipperException : public std::exception
{
  public:
    clipperException(const char* description)
      throw(): std::exception(), m_description (description) {}
    virtual ~clipperException() throw() {}
    virtual const char* what() const throw() {return m_description.c_str();}
  private:
    std::string m_description;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

  struct SkipNode
  {
    void* item;
    int level;
    SkipNode* prev;    //ie SkipNodes form a double linked list
    SkipNode* next[1];
  };

  template<typename T>
  class SkipList
  {
    //definition of the compare function that's passed to the constructor ...
    typedef int (*compareFunc)(T, T);
  public:
    SkipList( compareFunc cf);
    ~SkipList();
    void Clear();
    SkipNode* InsertItem(T item); //throws an exception on duplicates
    SkipNode* FindItem(T item);
    bool DeleteItem(T item);
    void Delete(SkipNode*& node);
    T PopFirst();
    SkipNode* First();
    SkipNode* Next(SkipNode* currentNode);
    SkipNode* Prev(SkipNode* currentNode);
    SkipNode* Last();
    unsigned Count();
  private:
    int m_MaxLevel;
    double m_SkipFrac;
    int m_CurrentMaxLevel;
    SkipNode* m_Base;
    int m_Count;
    SkipNode* m_Lvls[32];
    SkipNode* NewNode(int level, T item);
    compareFunc m_CompareFunc;
  };

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class skiplistException : public std::exception
{
  public:
    skiplistException(const char* description)
      throw(): std::exception(), m_description (description) {}
    virtual ~skiplistException() throw() {}
    virtual const char* what() const throw() {return m_description.c_str();}
  private:
    std::string m_description;
};
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


} //clipper namespace
#endif //clipper_hpp


