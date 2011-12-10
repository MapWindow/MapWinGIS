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

#include "stdafx.h"
#include "clipper.h"
#include <cmath>
#include <ctime>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdlib>

namespace clipper {

static double const horizontal = -3.4E+38;
static double const unassigned = 3.4E+38;
//tolerance: is needed because vertices are floating point values and any
//comparison of floating point values requires a degree of tolerance.
static double const tolerance = 1.0E-10;
static double const minimal_tolerance = 1.0E-10;	//20
//static double const lowest_tolerance = 1.0E-20;	//20

//precision: defines when adjacent vertices will be considered duplicates
//and hence ignored. This circumvents edges having indeterminate slope.
static double const precision = 1.0E-6;
static double const slope_precision = 1.0E-3;
enum TDirection { dRightToLeft, dLeftToRight };
static const TDoubleRect nullRect = {0,0,0,0};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

template<typename T>
SkipList<T>::SkipList( compareFunc cf ): m_CompareFunc(cf)
{
  //Hard code a likely 'maximum' of 10 million items (since I can't imagine
  //more than 10M edges existing in any given scanbeam.). The list could still
  //grow beyond this number, but Skiplist performance would slowly degrade.
  //const double maxItems = 10000000;
  const double skip = 4;
  std::srand ( std::time(0) );

  m_MaxLevel = 12;//std::ceil( std::log(maxItems)/std::log(skip) );
  m_SkipFrac = 1/skip;

  //create and initialize the base node ...
  m_Base = NewNode(m_MaxLevel-1, 0);
  for (int i = 0; i < m_MaxLevel; ++i) m_Base->next[i] = 0;
  m_CurrentMaxLevel = 0;
  m_Count = 0;
  m_Base->prev = m_Base;
}
//------------------------------------------------------------------------------

template<typename T>
SkipList<T>::~SkipList()
{
  Clear();
  delete m_Base;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::NewNode(int level, T item)
{
  void* buf = std::malloc(sizeof(SkipNode)+ level*sizeof(void*));
  SkipNode* result = new (buf) SkipNode;
  result->level = level;
  result->item = item;
  m_Count++;
  return result;
}
//------------------------------------------------------------------------------

template<typename T>
void SkipList<T>::Clear()
{
  SkipNode* tmp = m_Base->prev;
  while (tmp != m_Base)
  {
    SkipNode* tmp2 = tmp->prev;
    delete tmp;
    tmp = tmp2;
  }
  for (int i = 0; i < m_MaxLevel; ++i) m_Base->next[i] = 0;
  m_CurrentMaxLevel = 0;
  m_Base->prev = m_Base;
  m_Count = 0;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::InsertItem(T item)
{
  for (int i = 0; i < m_MaxLevel; ++i) m_Lvls[i] = m_Base;
  SkipNode* priorNode = m_Base;

  int compareVal = 1;
  for (int i = m_CurrentMaxLevel; i >= 0; --i)
  {
    while (priorNode->next[i])
    {
      //avoid a few unnecessary compares ...
      if (compareVal > 0 || priorNode->next[i+1] != priorNode->next[i])
        compareVal = m_CompareFunc((T)priorNode->next[i]->item, (T)item);
      if (compareVal > 0) priorNode = priorNode->next[i];
      else break;
    }
    m_Lvls[i] = priorNode;
  }

  if (compareVal == 0)
    throw skiplistException("Skiplist error: Duplicate items not allowed.");

  //get the level of the new node ...
  int newLevel = 0;
  while ( newLevel <= m_CurrentMaxLevel && newLevel < m_MaxLevel-1 &&
    (std::rand() % 1000) < m_SkipFrac*1000 ) newLevel++;
  if (newLevel > m_CurrentMaxLevel) m_CurrentMaxLevel = newLevel;

  //create the new node and rearrange links up to newLevel ...
  SkipNode* result = NewNode(newLevel, item);
  if (priorNode->next[0])
    priorNode->next[0]->prev = result; else
    m_Base->prev = result; //fBase.prev always points to the last node
  result->prev = priorNode;
  for (int i = 0; i <= newLevel; ++i)
  {
    result->next[i] = m_Lvls[i]->next[i];
    m_Lvls[i]->next[i] = result;
  }
  return result;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::FindItem(T item)
{
  SkipNode* result = m_Base;
  int compareVal = 1;
  for (int i = m_CurrentMaxLevel; i >= 0; --i)
  {
    while (result->next[i])
    {
      if (compareVal > 0 || result->next[i+1] != result->next[i])
        compareVal = m_CompareFunc((T)result->next[i]->item, (T)item);
      if (compareVal <= 0) break;
      else result = result->next[i];
    }
    if (compareVal == 0) return result->next[i];
  }
  return 0;
}
//------------------------------------------------------------------------------

template<typename T>
bool SkipList<T>::DeleteItem(T item)
{
  for (int i = 0; i <= m_CurrentMaxLevel; ++i) m_Lvls[i] = m_Base;
  SkipNode* priorNode = m_Base;

  //find the item ...
  int compareVal = 1;
  for (int i = m_CurrentMaxLevel; i >= 0; --i)
  {
    while (priorNode->next[i])
    {
      if (compareVal > 0 || priorNode->next[i+1] != priorNode->next[i])
        compareVal = m_CompareFunc((T)priorNode->next[i]->item, (T)item);
      if (compareVal > 0) priorNode = priorNode->next[i];
      else break;
    }
    m_Lvls[i] = priorNode;
  }
  if (compareVal != 0) return false;

  SkipNode* delNode = priorNode->next[0];
  //if this's the only node at fCurrentMaxLevel, decrement fCurrentMaxLevel ...
  if (delNode->level > 0 && delNode->level == m_CurrentMaxLevel &&
    m_Lvls[delNode->level] == m_Base && !delNode->next[delNode->level])
      m_CurrentMaxLevel--;

  //fix up links before finally deleting the node ...
  for (int i = 0; i <= delNode->level; ++i)
    m_Lvls[i]->next[i] = delNode->next[i];
  if (delNode->next[0])
    delNode->next[0]->prev = delNode->prev; else
    m_Base->prev = delNode->prev;
  delete delNode;
  m_Count--;
  return true;
}
//------------------------------------------------------------------------------

template<typename T>
void SkipList<T>::Delete(SkipNode*& node)
{
  //this method doesn't call m_CompareFunc() ...
  for (int i = node->level +1; i <= m_CurrentMaxLevel; ++i) m_Lvls[i] = 0;
  SkipNode* nextNode = node;
  int lvl = node->level;
  while (nextNode->next[nextNode->level] &&
    nextNode->level < m_CurrentMaxLevel)
  {
    nextNode = nextNode->next[nextNode->level];
    while (nextNode->level > lvl)
    {
      lvl++;
      m_Lvls[lvl] = nextNode;
    }
  }
  for (int i = 0;  i <= node->level; ++i) m_Lvls[i] = node;

  SkipNode* priorNode = m_Base;
  for (int i = m_CurrentMaxLevel; i >= 0; --i)
  {
    while ( priorNode->next[i] && priorNode->next[i] != m_Lvls[i] )
      priorNode = priorNode->next[i];
    m_Lvls[i] = priorNode;
  }

  //if this's the only node at fCurrentMaxLevel, decrement fCurrentMaxLevel ...
  if (node->level > 0 && node->level == m_CurrentMaxLevel &&
  m_Lvls[node->level] == m_Base && !node->next[node->level])
    m_CurrentMaxLevel--;

  //fix up links before finally deleting the node ...
  for (int i = 0; i <= node->level; ++i)
    m_Lvls[i]->next[i] = node->next[i];
  if (node->next[0])
    node->next[0]->prev = node->prev; else
    m_Base->prev = node->prev;
  delete node;
  m_Count--;
  node = 0;
}
//------------------------------------------------------------------------------

template<typename T>
T SkipList<T>::PopFirst()
{
  if (m_Count == 0) return 0;
  SkipNode* delNode = m_Base->next[0];
  T result = (T)delNode->item;

  int delLevel = delNode->level;
  //if this's the only node at fCurrentMaxLevel, decrement fCurrentMaxLevel ...
  if (delLevel > 0 && delLevel == m_CurrentMaxLevel &&
    m_Base->next[delLevel] == delNode && !delNode->next[delLevel])
      m_CurrentMaxLevel--;

  //fix up links before finally deleting the node ...
  for (int i = 0; i <= delLevel; ++i) m_Base->next[i] = delNode->next[i];
  if (delNode->next[0])
    delNode->next[0]->prev = m_Base; else m_Base->prev = m_Base;
  delete delNode;
  m_Count--;
  return result;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::First()
{
  if (m_Count == 0) return 0; else return m_Base->next[0];
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::Next(SkipNode* currentNode)
{
  if (currentNode) return currentNode->next[0];
  else return 0;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::Prev(SkipNode* currentNode)
{
  if (currentNode)
  {
    if (currentNode->prev == m_Base) return 0;
    else return currentNode->prev;
  }
  else return 0;
}
//------------------------------------------------------------------------------

template<typename T>
SkipNode* SkipList<T>::Last()
{
  if (m_Base->prev == m_Base) return 0;
  else return m_Base->prev;
}
//------------------------------------------------------------------------------

template<typename T>
unsigned SkipList<T>::Count()
{
  return m_Count;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TDoubleRect GetBounds(const TPolygon& poly)
{
  if (poly.size() == 0) return nullRect;
  TDoubleRect result;
  result.left = poly[0].X; result.top = poly[0].Y;
  result.right = poly[0].X; result.bottom = poly[0].Y;
  for (int i = 1; i < int(poly.size()); ++i)
  {
    if (poly[i].X < result.left) result.left = poly[i].X;
    else if (poly[i].X > result.right) result.right = poly[i].X;
    if (poly[i].Y < result.top) result.top = poly[i].Y;
    else if (poly[i].Y > result.bottom) result.bottom = poly[i].Y;
  }
  return result;
}
//------------------------------------------------------------------------------

bool IsClockwise(const TPolygon &poly)
{
  int highI = poly.size() -1;
  if (highI < 2) return false;
  double area = poly[highI].X * poly[0].Y - poly[0].X * poly[highI].Y;
  for (int i = 0; i < highI; ++i)
    area += poly[i].X * poly[i+1].Y - poly[i+1].X * poly[i].Y;
  //area := area/2;
  return area > 0; //ie reverse of normal formula because Y axis inverted
}
//------------------------------------------------------------------------------

TDoublePoint DoublePoint(const double &X, const double &Y)
{
  TDoublePoint p;
  p.X = X;
  p.Y = Y;
  return p;
}
//------------------------------------------------------------------------------

bool PointsEqual( const TDoublePoint &pt1, const TDoublePoint &pt2)
{
  return ( std::fabs( pt1.X - pt2.X ) < precision + tolerance ) &&
  ( std::fabs( (pt1.Y - pt2.Y) ) < precision + tolerance );
}
//------------------------------------------------------------------------------

bool PointsEqual( const double &pt1x, const double &pt1y,
  const double &pt2x, const double &pt2y)
{
  return ( std::fabs( pt1x - pt2x ) < precision + tolerance ) &&
  ( std::fabs( (pt1y - pt2y) ) < precision + tolerance );
}
//------------------------------------------------------------------------------

TDoublePoint GetUnitNormal( const TDoublePoint &pt1, const TDoublePoint &pt2)
{
  double dx = ( pt2.X - pt1.X );
  double dy = ( pt2.Y - pt1.Y );
  if(  ( dx == 0 ) && ( dy == 0 ) ) return DoublePoint( 0, 0 );

  double f = 1 *1.0/ std::sqrt( dx*dx + dy*dy );
  dx = dx * f;
  dy = dy * f;
  return DoublePoint(dy, -dx);
}
//------------------------------------------------------------------------------

void DisposePolyPts(TPolyPt*& pp)
{
  if (pp == 0) return;
  TPolyPt *tmpPp;
  pp->prev->next = 0;
  while( pp )
  {
    tmpPp = pp;
    pp = pp->next;
    delete tmpPp ;
  }
}
//------------------------------------------------------------------------------

void Clipper::DisposeAllPolyPts(){
  for (PolyPtList::size_type i = 0; i < m_PolyPts.size(); ++i)
    DisposePolyPts(m_PolyPts[i]);
  m_PolyPts.clear();
}
//------------------------------------------------------------------------------

TPolyPt* DuplicatePolyPt(TPolyPt* polyPt)
{
  TPolyPt* result = new TPolyPt;
  result->pt = polyPt->pt;
  result->flags = polyPt->flags;
  result->dx = unassigned;
  result->prev = polyPt;
  result->next = polyPt->next;
  polyPt->next->prev = result;
  polyPt->next = result;
  return result;
}
//------------------------------------------------------------------------------

bool PtIsAPolyPt(const TDoublePoint pt, TPolyPt*& polyStartPt)
{
  if (!polyStartPt) return false;
  TPolyPt* p = polyStartPt;
  do {
    if (PointsEqual(pt, polyStartPt->pt)) return true;
    polyStartPt = polyStartPt->next;
  }
  while (polyStartPt != p);
  return false;
}
//------------------------------------------------------------------------------

void ReversePolyPtLinks(TPolyPt &pp)
{
  TPolyPt *pp1, *pp2;
  pp1 = &pp;
  do {
  pp2 = pp1->next;
  pp1->next = pp1->prev;
  pp1->prev = pp2;
  pp1 = pp2;
  } while( pp1 != &pp );
}
//------------------------------------------------------------------------------

bool IsHorizontal(const TEdge &e)
{
  return &e  && ( e.dx == horizontal );
}
//------------------------------------------------------------------------------

bool IsHorizontal(TDoublePoint pt1, TDoublePoint pt2)
{
  return (std::fabs(pt1.X - pt2.X) > precision &&
    std::fabs(pt1.Y - pt2.Y) < precision);
}
//------------------------------------------------------------------------------

void SetDx(TEdge &e)
{
  double dx = std::fabs(e.xbot - e.next->xbot);
  double dy = std::fabs(e.ybot - e.next->ybot);
  if (dx == 0) e.dx = 0;
  //Very short, nearly horizontal edges can cause problems by very
  //inaccurately determining intermediate X values - see TopX().
  //Therefore treat very short, nearly horizontal edges as horizontal too ...
  else if ( (dx < 0.1 && dy *10 < dx) || dy < slope_precision )
  {
    e.dx = horizontal;
    if (e.ybot != e.next->ybot) e.ybot = e.next->ybot;
  }
  else e.dx = (e.xbot - e.next->xbot)/(e.ybot - e.next->ybot);
}
//------------------------------------------------------------------------------

void SwapSides(TEdge &edge1, TEdge &edge2)
{
  TEdgeSide side =  edge1.side;
  edge1.side = edge2.side;
  edge2.side = side;
}
//------------------------------------------------------------------------------

void SwapPolyIndexes(TEdge &edge1, TEdge &edge2)
{
  int outIdx =  edge1.outIdx;
  edge1.outIdx = edge2.outIdx;
  edge2.outIdx = outIdx;
}
//------------------------------------------------------------------------------

double TopX(TEdge *edge, const double &currentY)
{
  if(  currentY == edge->ytop ) return edge->xtop;
  return edge->xbot + edge->dx *( currentY - edge->ybot );
}
//------------------------------------------------------------------------------

bool EdgesShareSamePoly(TEdge &e1, TEdge &e2)
{
  return &e1  && &e2  && ( e1.outIdx == e2.outIdx );
}
//------------------------------------------------------------------------------

bool SlopesEqual(TEdge &e1, TEdge &e2)
{
  if (IsHorizontal(e1)) return IsHorizontal(e2);
  if (IsHorizontal(e2)) return false;
  return std::fabs((e1.ytop - e1.ybot)*(e2.xtop - e2.xbot) -
      (e1.xtop - e1.xbot)*(e2.ytop - e2.ybot)) < slope_precision;
}
//------------------------------------------------------------------------------

bool SlopesEqual(const TDoublePoint pt1,
  const TDoublePoint pt2, const TDoublePoint pt3)
{
  return (std::fabs(pt1.X-pt2.X) <= slope_precision &&
      std::fabs(pt2.X-pt3.X) <= slope_precision) ||
      (std::fabs(pt1.Y-pt2.Y) <= slope_precision &&
      std::fabs(pt2.Y-pt3.Y) <= slope_precision) ||
      (std::fabs((pt2.Y - pt1.Y)*(pt3.X - pt2.X) -
      (pt2.X - pt1.X)*(pt3.Y - pt2.Y)) < slope_precision);
}
//------------------------------------------------------------------------------

bool IntersectPoint(TEdge &edge1, TEdge &edge2, TDoublePoint &ip)
{
  double b1, b2;
  if(  edge1.dx == 0 )
  {
    ip.X = edge1.xbot;
    b2 = edge2.ybot - edge2.xbot/edge2.dx;
    ip.Y = ip.X/edge2.dx + b2;
  }
  else if(  edge2.dx == 0 )
  {
    ip.X = edge2.xbot;
    b1 = edge1.ybot - edge1.xbot/edge1.dx;
    ip.Y = ip.X/edge1.dx + b1;
  }
  else
  {
    if( edge1.dx == edge2.dx ) return false;
    b1 = edge1.xbot - edge1.ybot *edge1.dx;
    b2 = edge2.xbot - edge2.ybot *edge2.dx;
    ip.Y = (b2-b1)/(edge1.dx - edge2.dx);
    ip.X = edge1.dx * ip.Y + b1;
  }
  return (ip.Y > edge1.ytop + tolerance) && (ip.Y > edge2.ytop + tolerance);
}
//------------------------------------------------------------------------------

bool IsClockwise(TPolyPt *pt)
{
  double area = 0;
  TPolyPt* startPt = pt;
  do
  {
    area = area + (pt->pt.X * pt->next->pt.Y) - (pt->next->pt.X * pt->pt.Y);
    pt = pt->next;
  }
  while (pt != startPt);
  //area = area /2;
  return area > 0; //ie reverse of normal formula because Y axis inverted
}
//------------------------------------------------------------------------------

void InitEdge(TEdge *e, TEdge *eNext, TEdge *ePrev, const TDoublePoint &pt)
{
  std::memset( e, 0, sizeof( TEdge ));
  e->xbot = pt.X;
  e->ybot = pt.Y;
  e->next = eNext;
  e->prev = ePrev;
  SetDx(*e);
}
//------------------------------------------------------------------------------

void ReInitEdge(TEdge *e, const double &nextX,
  const double &nextY, TPolyType polyType)
{
  if ( e->ybot > nextY )
  {
    e->xcurr = e->xbot;
    e->ycurr = e->ybot;
    e->xtop = nextX;
    e->ytop = nextY;
    e->nextAtTop = true;
  } else {
    e->xcurr = nextX;
    e->ycurr = nextY;
    e->xtop = e->xbot;
    e->ytop = e->ybot;
    e->xbot = e->xcurr;
    e->ybot = e->ycurr;
    e->nextAtTop = false;
  }
  e->polyType = polyType;
  e->outIdx = -1;
}
//------------------------------------------------------------------------------

bool SlopesEqualInternal(TEdge &e1, TEdge &e2)
{
  if (IsHorizontal(e1)) return IsHorizontal(e2);
  if (IsHorizontal(e2)) return false;
  return std::fabs((e1.ybot - e1.next->ybot) *
      (e2.xbot - e2.next->xbot) -
      (e1.xbot - e1.next->xbot) *
      (e2.ybot - e2.next->ybot)) < slope_precision;
}
//------------------------------------------------------------------------------

void FixupForDupsAndColinear(TEdge *edges)
{
  TEdge* lastOK = 0;
  TEdge *e = edges;
  for (;;) {
    if (e->next == e->prev) break;
    else if (PointsEqual(e->prev->xbot, e->prev->ybot, e->xbot, e->ybot) ||
      SlopesEqualInternal(*e->prev, *e))
    {
      lastOK = 0;
      //remove 'e' from the double-linked-list ...
      if ( e == edges )
      {
        //move the content of e.next to e before removing e.next from DLL ...
        e->xbot = e->next->xbot;
        e->ybot = e->next->ybot;
        e->next->next->prev = e;
        e->next = e->next->next;
      } else
      {
        //remove 'e' from the loop ...
        e->prev->next = e->next;
        e->next->prev = e->prev;
        e = e->prev; //ie get back into the loop
      }
      SetDx(*e->prev);
      SetDx(*e);
    }
    else if (lastOK == e) break;
    else
    {
        if (lastOK == 0) lastOK = e;
        e = e->next;
    }
  }
}
//------------------------------------------------------------------------------

void SwapX(TEdge &e)
{
  //swap horizontal edges' top and bottom x's so they follow the natural
  //progression of the bounds - ie so their xbots will align with the
  //adjoining lower edge. [Helpful in the ProcessHorizontal() method.]
  e.xcurr = e.xtop;
  e.xtop = e.xbot;
  e.xbot = e.xcurr;
  e.nextAtTop = !e.nextAtTop; //but really redundant for horizontals
}

//------------------------------------------------------------------------------
// ClipperBase methods ...
//------------------------------------------------------------------------------

ClipperBase::ClipperBase() //constructor
{
  m_localMinimaList = 0;
  m_CurrentLM = 0;
  m_edges.reserve(32);
}
//------------------------------------------------------------------------------

ClipperBase::~ClipperBase() //destructor
{
  Clear();
}
//------------------------------------------------------------------------------

void ClipperBase::InsertLocalMinima(TLocalMinima *newLm)
{
  if( ! m_localMinimaList )
  {
    m_localMinimaList = newLm;
  }
  else if( newLm->Y >= m_localMinimaList->Y )
  {
    newLm->nextLm = m_localMinimaList;
    m_localMinimaList = newLm;
  } else
  {
    TLocalMinima* tmpLm = m_localMinimaList;
    while( tmpLm->nextLm  && ( newLm->Y < tmpLm->nextLm->Y ) )
      tmpLm = tmpLm->nextLm;
    newLm->nextLm = tmpLm->nextLm;
    tmpLm->nextLm = newLm;
  }
}
//------------------------------------------------------------------------------

TEdge* ClipperBase::AddBoundsToLML(TEdge *e)
{
  //Starting at the top of one bound we progress to the bottom where there's
  //a local minima. We then go to the top of the next bound. These two bounds
  //form the left and right (or right and left) bounds of the local minima.
  e->nextInLML = 0;
  e = e->next;
  for (;;)
  {
    if ( IsHorizontal(*e) )
    {
      //nb: proceed through horizontals when approaching from their right,
      //    but break on horizontal minima if approaching from their left.
      //    This ensures 'local minima' are always on the left of horizontals.
      if (e->next->ytop < e->ytop && e->next->xcurr > e->prev->xcurr) break;
      if (e->xtop != e->prev->xcurr) SwapX( *e );
      e->nextInLML = e->prev;
    }
    else if (e->ycurr == e->prev->ycurr) break;
    else e->nextInLML = e->prev;
    e = e->next;
  }

  //e and e.prev are now at a local minima ...
  TLocalMinima* newLm = new TLocalMinima;
  newLm->nextLm = 0;
  newLm->Y = e->prev->ycurr;

  if ( IsHorizontal(*e) ) //horizontal edges never start a left bound
  {
    if (e->xcurr != e->prev->xcurr) SwapX(*e);
    newLm->leftBound = e->prev;
    newLm->rightBound = e;
  } else if (e->dx < e->prev->dx)
  {
    newLm->leftBound = e->prev;
    newLm->rightBound = e;
  } else
  {
    newLm->leftBound = e;
    newLm->rightBound = e->prev;
  }
  newLm->leftBound->side = esLeft;
  newLm->rightBound->side = esRight;
  InsertLocalMinima( newLm );

  for (;;)
  {
    if ( e->next->ytop == e->ytop && !IsHorizontal(*e->next) ) break;
    e->nextInLML = e->next;
    e = e->next;
    if ( IsHorizontal(*e) && e->xcurr != e->prev->xtop) SwapX(*e);
  }
  return e->next;
}
//------------------------------------------------------------------------------

TDoublePoint RoundToPrecision(const TDoublePoint &pt){
  TDoublePoint result;
  result.X = (pt.X >= 0.0) ?
    (std::floor( pt.X/precision + 0.5 ) * precision):
    (std::ceil( pt.X/precision + 0.5 ) * precision);
  result.Y = (pt.Y >= 0.0) ?
    (std::floor( pt.Y/precision + 0.5 ) * precision):
    (std::ceil( pt.Y/precision + 0.5 ) * precision);
  return result;
}
//------------------------------------------------------------------------------

void ClipperBase::AddPolygon( const TPolygon &pg, TPolyType polyType)
{
  int highI = pg.size() -1;
  TPolygon p(highI + 1);
  for (int i = 0; i <= highI; ++i) p[i] = RoundToPrecision(pg[i]);
  while( (highI > 1) && PointsEqual(p[0] , p[highI]) ) highI--;
  if(  highI < 2 ) return;

  //make sure this is still a sensible polygon (ie with at least one minima) ...
  int i = 1;
  while(  i <= highI && std::fabs(p[i].Y - p[0].Y) < precision ) i++;
  if( i > highI ) return;

  //create a new edge array ...
  TEdge *edges = new TEdge [highI +1];
  m_edges.push_back(edges);

  //convert 'edges' to a double-linked-list and initialize a few of the vars ...
  edges[0].xbot = p[0].X;
  edges[0].ybot = p[0].Y;
  InitEdge(&edges[highI], &edges[0], &edges[highI-1], p[highI]);
  for (i = highI-1; i > 0; --i)
    InitEdge(&edges[i], &edges[i+1], &edges[i-1], p[i]);
  InitEdge(&edges[0], &edges[1], &edges[highI], p[0]);
  TEdge* e = &edges[highI];
  while (IsHorizontal(*e) && e !=&edges[0]) {
    if (e->ybot != e->next->ybot) e->ybot = e->next->ybot;
    e = e->prev;
  }

  //fixup by deleting duplicate points and merging co-linear edges ...
  FixupForDupsAndColinear(edges);

  //make sure we still have a valid polygon ...
  e = edges;
  if( e->next == e->prev )
  {
    m_edges.pop_back();
    delete [] edges;
    return;
  }

  //now properly re-initialize edges and also find 'eHighest' ...
  e = edges->next;
  TEdge* eHighest = e;
  do {
    ReInitEdge(e, e->next->xbot, e->next->ybot, polyType);
    if(  e->ytop < eHighest->ytop ) eHighest = e;
    e = e->next;
  } while( e != edges );

  if ( e->next->nextAtTop )
    ReInitEdge(e, e->next->xbot, e->next->ybot, polyType); else
    ReInitEdge(e, e->next->xtop, e->next->ytop, polyType);
  if ( e->ytop < eHighest->ytop ) eHighest = e;

  //make sure eHighest is positioned so the following loop works safely ...
  if ( eHighest->nextAtTop ) eHighest = eHighest->next;
  if ( IsHorizontal( *eHighest) ) eHighest = eHighest->next;

  //finally insert each local minima ...
  e = eHighest;
  do {
    e = AddBoundsToLML(e);
  } while( e != eHighest );

}
//------------------------------------------------------------------------------

void ClipperBase::AddPolyPolygon( const TPolyPolygon &ppg, TPolyType polyType)
{
  for (TPolyPolygon::size_type i = 0; i < ppg.size(); ++i)
  AddPolygon(ppg[i], polyType);
}
//------------------------------------------------------------------------------

void ClipperBase::Clear()
{
  DisposeLocalMinimaList();
  for (EdgeList::size_type i = 0; i < m_edges.size(); ++i) delete [] m_edges[i];
  m_edges.clear();
}
//------------------------------------------------------------------------------

TDoubleRect ClipperBase::GetBounds()
{
  TDoubleRect result;
  TLocalMinima* lm = m_localMinimaList;
  if (!lm)
  {
    result.left = 0;
    result.top = 0;
    result.right = 0;
    result.bottom = 0;
    return result;
  }
  result.left = unassigned;
  result.top = unassigned;
  result.right = -unassigned;
  result.bottom = -unassigned;
  while (lm)
  {
    if (lm->leftBound->ybot > result.bottom) result.bottom = lm->leftBound->ybot;
    TEdge* e = lm->leftBound;
    while (e->nextInLML)
    {
      if (e->xbot < result.left) result.left = e->xbot;
      e = e->nextInLML;
    }
    if (e->xbot < result.left) result.left = e->xbot;
    else if (e->xtop < result.left) result.left = e->xtop;
    if (e->ytop < result.top) result.top = e->ytop;

    e = lm->rightBound;
    while (e->nextInLML)
    {
      if (e->xbot > result.right) result.right = e->xbot;
      e = e->nextInLML;
    }
    if (e->xbot > result.right) result.right = e->xbot;
    else if (e->xtop > result.right) result.right = e->xtop;

    lm = lm->nextLm;
  }
  return result;
}
//------------------------------------------------------------------------------

bool ClipperBase::Reset()
{
  m_CurrentLM = m_localMinimaList;
  if( !m_CurrentLM ) return false; //ie nothing to process

  //reset all edges ...
  TLocalMinima* lm = m_localMinimaList;
  while( lm )
  {
    TEdge* e = lm->leftBound;
    while( e )
    {
      e->xcurr = e->xbot;
      e->ycurr = e->ybot;
      e->side = esLeft;
      e->outIdx = -1;
      e = e->nextInLML;
    }
    e = lm->rightBound;
    while( e )
    {
      e->xcurr = e->xbot;
      e->ycurr = e->ybot;
      e->side = esRight;
      e->outIdx = -1;
      e = e->nextInLML;
    }
    lm = lm->nextLm;
  }
  return true;
}
//------------------------------------------------------------------------------

void ClipperBase::PopLocalMinima()
{
  if( ! m_CurrentLM ) return;
  m_CurrentLM = m_CurrentLM->nextLm;
}
//------------------------------------------------------------------------------

void ClipperBase::DisposeLocalMinimaList()
{
  while( m_localMinimaList )
  {
    TLocalMinima* tmpLm = m_localMinimaList->nextLm;
    delete m_localMinimaList;
    m_localMinimaList = tmpLm;
  }
  m_CurrentLM = 0;
}

//------------------------------------------------------------------------------
// Clipper methods ...
//------------------------------------------------------------------------------

Clipper::Clipper() : ClipperBase() //constructor
{
  m_Scanbeam = 0;
  m_ActiveEdges = 0;
  m_SortedEdges = 0;
  m_IntersectNodes = 0;
  m_ExecuteLocked = false;
  m_IgnoreOrientation = false;
  m_PolyPts.reserve(32);
};
//------------------------------------------------------------------------------

Clipper::~Clipper() //destructor
{
  DisposeScanbeamList();
  DisposeAllPolyPts();
};
//------------------------------------------------------------------------------

void Clipper::DisposeScanbeamList()
{
  while ( m_Scanbeam ) {
  TScanbeam* sb2 = m_Scanbeam->nextSb;
  delete m_Scanbeam;
  m_Scanbeam = sb2;
  }
}
//------------------------------------------------------------------------------

bool Clipper::InitializeScanbeam()
{
  DisposeScanbeamList();
  if(  !Reset() ) return false;
  //add all the local minima into a fresh fScanbeam list ...
  TLocalMinima* lm = m_CurrentLM;
  while( lm )
  {
  InsertScanbeam( lm->Y );
  InsertScanbeam(lm->leftBound->ytop); //this is necessary too!
  lm = lm->nextLm;
  }
  return true;
}
//------------------------------------------------------------------------------

void Clipper::InsertScanbeam( const double &Y)
{
  if( !m_Scanbeam )
  {
    m_Scanbeam = new TScanbeam;
    m_Scanbeam->nextSb = 0;
    m_Scanbeam->Y = Y;
  }
  else if(  Y > m_Scanbeam->Y )
  {
    TScanbeam* newSb = new TScanbeam;
    newSb->Y = Y;
    newSb->nextSb = m_Scanbeam;
    m_Scanbeam = newSb;
  } else
  {
    TScanbeam* sb2 = m_Scanbeam;
    while( sb2->nextSb  && ( Y <= sb2->nextSb->Y ) ) sb2 = sb2->nextSb;
    if(  Y == sb2->Y ) return; //ie ignores duplicates
    TScanbeam* newSb = new TScanbeam;
    newSb->Y = Y;
    newSb->nextSb = sb2->nextSb;
    sb2->nextSb = newSb;
  }
}
//------------------------------------------------------------------------------

double Clipper::PopScanbeam()
{
  double Y = m_Scanbeam->Y;
  TScanbeam* sb2 = m_Scanbeam;
  m_Scanbeam = m_Scanbeam->nextSb;
  delete sb2;
  return Y;
}
//------------------------------------------------------------------------------

void Clipper::SetWindingDelta(TEdge& edge)
{
  if ( !IsNonZeroFillType(edge) ) edge.windDelta = 1;
  else if ( edge.nextAtTop ) edge.windDelta = 1;
  else edge.windDelta = -1;
}
//------------------------------------------------------------------------------

void Clipper::SetWindingCount(TEdge& edge)
{
  TEdge* e = edge.prevInAEL;
  //find the edge of the same polytype that immediately preceeds 'edge' in AEL
  while ( e  && e->polyType != edge.polyType ) e = e->prevInAEL;
  if ( !e )
  {
    edge.windCnt = edge.windDelta;
    edge.windCnt2 = 0;
    e = m_ActiveEdges; //ie get ready to calc windCnt2
  } else if ( IsNonZeroFillType(edge) )
  {
    //nonZero filling ...
    if ( e->windCnt * e->windDelta < 0 )
    {
      if (std::abs(e->windCnt) > 1)
      {
        if (e->windDelta * edge.windDelta < 0) edge.windCnt = e->windCnt;
        else edge.windCnt = e->windCnt + edge.windDelta;
      } else
        edge.windCnt = e->windCnt + e->windDelta + edge.windDelta;
    } else
    {
      if ( std::abs(e->windCnt) > 1 && e->windDelta * edge.windDelta < 0)
        edge.windCnt = e->windCnt;
      else if ( e->windCnt + edge.windDelta == 0 )
        edge.windCnt = e->windCnt;
      else edge.windCnt = e->windCnt + edge.windDelta;
    }
    edge.windCnt2 = e->windCnt2;
    e = e->nextInAEL; //ie get ready to calc windCnt2
  } else
  {
    //even-odd filling ...
    edge.windCnt = 1;
    edge.windCnt2 = e->windCnt2;
    e = e->nextInAEL; //ie get ready to calc windCnt2
  }

  //update windCnt2 ...
  if ( IsNonZeroAltFillType(edge) )
  {
    //nonZero filling ...
    while ( e != &edge )
    {
      edge.windCnt2 += e->windDelta;
      e = e->nextInAEL;
    }
  } else
  {
    //even-odd filling ...
    while ( e != &edge )
    {
      edge.windCnt2 = (edge.windCnt2 == 0) ? 1 : 0;
      e = e->nextInAEL;
    }
  }
}
//------------------------------------------------------------------------------

bool Clipper::IsNonZeroFillType(const TEdge& edge) const
{
  switch (edge.polyType) {
    case ptSubject: return m_SubjFillType == pftNonZero;
  default: return m_ClipFillType == pftNonZero;
  }
}
//------------------------------------------------------------------------------

bool Clipper::IsNonZeroAltFillType(const TEdge& edge) const
{
  switch (edge.polyType) {
    case ptSubject: return m_ClipFillType == pftNonZero;
  default: return m_SubjFillType == pftNonZero;
  }
}
//------------------------------------------------------------------------------

bool Edge2InsertsBeforeEdge1(TEdge &e1, TEdge &e2)
{
  if( e2.xcurr - tolerance > e1.xcurr ) return false;
  if( e2.xcurr + tolerance < e1.xcurr ) return true;
  if( IsHorizontal(e2) ) return false;
  return (e2.dx > e1.dx);
}
//------------------------------------------------------------------------------

void Clipper::InsertEdgeIntoAEL(TEdge *edge)
{
  edge->prevInAEL = 0;
  edge->nextInAEL = 0;
  if(  !m_ActiveEdges )
  {
    m_ActiveEdges = edge;
  }
  else if( Edge2InsertsBeforeEdge1(*m_ActiveEdges, *edge) )
  {
    edge->nextInAEL = m_ActiveEdges;
    m_ActiveEdges->prevInAEL = edge;
    m_ActiveEdges = edge;
  } else
  {
    TEdge* e = m_ActiveEdges;
    while( e->nextInAEL  && !Edge2InsertsBeforeEdge1(*e->nextInAEL , *edge) )
      e = e->nextInAEL;
    edge->nextInAEL = e->nextInAEL;
    if( e->nextInAEL ) e->nextInAEL->prevInAEL = edge;
    edge->prevInAEL = e;
    e->nextInAEL = edge;
  }
}
//----------------------------------------------------------------------

bool HorizOverlap(const double h1a,
  const double h1b, const double h2a, const double h2b)
{
  //returns true if (h1a between h2a and h2b) or
  //  (h1a == min2 and h1b > min2) or (h1a == max2 and h1b < max2)
  double min2, max2;
  if (h2a < h2b)
  {
    min2 = h2a;
    max2 = h2b;
  }
  else
  {
    min2 = h2b;
    max2 = h2a;
  }
  return (h1a > min2 + tolerance && h1a < max2 - tolerance) ||
    (std::fabs(h1a - min2) < tolerance && h1b > min2 + tolerance) ||
    (std::fabs(h1a - max2) < tolerance && h1b < max2 - tolerance);
}
//------------------------------------------------------------------------------

void Clipper::InsertLocalMinimaIntoAEL( const double &botY)
{
  while(  m_CurrentLM  && ( m_CurrentLM->Y == botY ) )
  {
    InsertEdgeIntoAEL( m_CurrentLM->leftBound );
    InsertScanbeam( m_CurrentLM->leftBound->ytop );
    InsertEdgeIntoAEL( m_CurrentLM->rightBound );

    SetWindingDelta( *m_CurrentLM->leftBound );
    if ( IsNonZeroFillType( *m_CurrentLM->leftBound) )
      m_CurrentLM->rightBound->windDelta =
        -m_CurrentLM->leftBound->windDelta; else
      m_CurrentLM->rightBound->windDelta = 1;

    SetWindingCount( *m_CurrentLM->leftBound );
    m_CurrentLM->rightBound->windCnt =
      m_CurrentLM->leftBound->windCnt;
    m_CurrentLM->rightBound->windCnt2 =
      m_CurrentLM->leftBound->windCnt2;

    if(  IsHorizontal( *m_CurrentLM->rightBound ) )
    {
      //nb: only rightbounds can have a horizontal bottom edge
      AddEdgeToSEL( m_CurrentLM->rightBound );
      InsertScanbeam( m_CurrentLM->rightBound->nextInLML->ytop );
    }
    else
      InsertScanbeam( m_CurrentLM->rightBound->ytop );

    TLocalMinima* lm = m_CurrentLM;
    if( IsContributing(*lm->leftBound) )
      AddLocalMinPoly( lm->leftBound,
        lm->rightBound, DoublePoint( lm->leftBound->xcurr , lm->Y ) );

    //flag polygons that share colinear edges, so they can be merged later ...
    if (lm->leftBound->outIdx >= 0 && lm->leftBound->prevInAEL &&
      lm->leftBound->prevInAEL->outIdx >= 0 &&
      std::fabs(lm->leftBound->prevInAEL->xcurr
        - lm->leftBound->xbot) < tolerance &&
      SlopesEqual(*lm->leftBound, *lm->leftBound->prevInAEL))
    {
      TDoublePoint pt = DoublePoint(lm->leftBound->xbot,lm->leftBound->ybot);
      AddPolyPt(lm->leftBound->prevInAEL, pt);
      int i = m_Joins.size();
      m_Joins.resize(i+1);
      m_Joins[i].idx1 = lm->leftBound->outIdx;
      m_Joins[i].idx2 = lm->leftBound->prevInAEL->outIdx;
      m_Joins[i].pt = pt;
    }
    if (lm->rightBound->outIdx >= 0 && IsHorizontal(*lm->rightBound))
    {
      //check for overlap with m_CurrentHorizontals
      for (JoinList::size_type i = 0; i < m_CurrentHorizontals.size(); ++i)
      {
        int hIdx = m_CurrentHorizontals[i].idx1;
        TDoublePoint hPt = m_CurrentHorizontals[i].outPPt->pt;
        TDoublePoint hPt2 = m_CurrentHorizontals[i].pt;
        TPolyPt* p = m_CurrentHorizontals[i].outPPt;

        TPolyPt* p2;
        if (IsHorizontal(p->pt, p->prev->pt) && (p->prev->pt.X == hPt2.X))
          p2 = p->prev;
        else if (IsHorizontal(p->pt, p->next->pt) && (p->next->pt.X == hPt2.X))
          p2 = p->next;
        else continue;

        if (HorizOverlap(hPt.X, hPt2.X, lm->rightBound->xbot, lm->rightBound->xtop))
        {
          AddPolyPt(lm->rightBound, hPt);
          int j = m_Joins.size();
          m_Joins.resize(j+1);
          m_Joins[j].idx1 = hIdx;
          m_Joins[j].idx2 = lm->rightBound->outIdx;
          m_Joins[j].pt = hPt;
        }
        else if (HorizOverlap(lm->rightBound->xbot,
          lm->rightBound->xtop, hPt.X, hPt2.X))
        {
          TDoublePoint pt =
            DoublePoint(lm->rightBound->xbot, lm->rightBound->ybot);
          int j = m_Joins.size();
          m_Joins.resize(j+1);
          if (!PointsEqual(pt, p->pt) && !PointsEqual(pt, p2->pt))
            InsertPolyPtBetween(pt, p, p2);
          m_Joins[j].idx1 = hIdx;
          m_Joins[j].idx2 = lm->rightBound->outIdx;
          m_Joins[j].pt = pt;

        }
      }
    }

    if( lm->leftBound->nextInAEL != lm->rightBound )
    {
      TEdge* e = lm->leftBound->nextInAEL;
      TDoublePoint pt = DoublePoint( lm->leftBound->xcurr, lm->leftBound->ycurr );
      while( e != lm->rightBound )
      {
        if(!e) throw clipperException("InsertLocalMinimaIntoAEL: missing rightbound!");
        //nb: For calculating winding counts etc, IntersectEdges() assumes
        //that param1 will be to the right of param2 ABOVE the intersection ...
        IntersectEdges( lm->rightBound , e , pt , ipNone); //order important here
        e = e->nextInAEL;
      }
    }
    PopLocalMinima();
  }
  m_CurrentHorizontals.clear();
}
//------------------------------------------------------------------------------

void Clipper::AddEdgeToSEL(TEdge *edge)
{
  //SEL pointers in PEdge are reused to build a list of horizontal edges.
  //However, we don't need to worry about order with horizontal edge processing.
  if( !m_SortedEdges )
  {
    m_SortedEdges = edge;
    edge->prevInSEL = 0;
    edge->nextInSEL = 0;
  }
  else
  {
    edge->nextInSEL = m_SortedEdges;
    edge->prevInSEL = 0;
    m_SortedEdges->prevInSEL = edge;
    m_SortedEdges = edge;
  }
}
//------------------------------------------------------------------------------

void Clipper::CopyAELToSEL()
{
  TEdge* e = m_ActiveEdges;
  m_SortedEdges = e;
  if (!m_ActiveEdges) return;
  m_SortedEdges->prevInSEL = 0;
  e = e->nextInAEL;
  while ( e )
  {
    e->prevInSEL = e->prevInAEL;
    e->prevInSEL->nextInSEL = e;
    e->nextInSEL = 0;
    e = e->nextInAEL;
  }
}
//------------------------------------------------------------------------------

void Clipper::SwapPositionsInAEL(TEdge *edge1, TEdge *edge2)
{
  if(  !( edge1->nextInAEL ) &&  !( edge1->prevInAEL ) ) return;
  if(  !( edge2->nextInAEL ) &&  !( edge2->prevInAEL ) ) return;

  if(  edge1->nextInAEL == edge2 )
  {
    TEdge* next = edge2->nextInAEL;
    if( next ) next->prevInAEL = edge1;
    TEdge* prev = edge1->prevInAEL;
    if( prev ) prev->nextInAEL = edge2;
    edge2->prevInAEL = prev;
    edge2->nextInAEL = edge1;
    edge1->prevInAEL = edge2;
    edge1->nextInAEL = next;
  }
  else if(  edge2->nextInAEL == edge1 )
  {
    TEdge* next = edge1->nextInAEL;
    if( next ) next->prevInAEL = edge2;
    TEdge* prev = edge2->prevInAEL;
    if( prev ) prev->nextInAEL = edge1;
    edge1->prevInAEL = prev;
    edge1->nextInAEL = edge2;
    edge2->prevInAEL = edge1;
    edge2->nextInAEL = next;
  }
  else
  {
    TEdge* next = edge1->nextInAEL;
    TEdge* prev = edge1->prevInAEL;
    edge1->nextInAEL = edge2->nextInAEL;
    if( edge1->nextInAEL ) edge1->nextInAEL->prevInAEL = edge1;
    edge1->prevInAEL = edge2->prevInAEL;
    if( edge1->prevInAEL ) edge1->prevInAEL->nextInAEL = edge1;
    edge2->nextInAEL = next;
    if( edge2->nextInAEL ) edge2->nextInAEL->prevInAEL = edge2;
    edge2->prevInAEL = prev;
    if( edge2->prevInAEL ) edge2->prevInAEL->nextInAEL = edge2;
  }

  if( !edge1->prevInAEL ) m_ActiveEdges = edge1;
  else if( !edge2->prevInAEL ) m_ActiveEdges = edge2;
}
//------------------------------------------------------------------------------

void Clipper::SwapPositionsInSEL(TEdge *edge1, TEdge *edge2)
{
  if(  !( edge1->nextInSEL ) &&  !( edge1->prevInSEL ) ) return;
  if(  !( edge2->nextInSEL ) &&  !( edge2->prevInSEL ) ) return;

  if(  edge1->nextInSEL == edge2 )
  {
    TEdge* next = edge2->nextInSEL;
    if( next ) next->prevInSEL = edge1;
    TEdge* prev = edge1->prevInSEL;
    if( prev ) prev->nextInSEL = edge2;
    edge2->prevInSEL = prev;
    edge2->nextInSEL = edge1;
    edge1->prevInSEL = edge2;
    edge1->nextInSEL = next;
  }
  else if(  edge2->nextInSEL == edge1 )
  {
    TEdge* next = edge1->nextInSEL;
    if( next ) next->prevInSEL = edge2;
    TEdge* prev = edge2->prevInSEL;
    if( prev ) prev->nextInSEL = edge1;
    edge1->prevInSEL = prev;
    edge1->nextInSEL = edge2;
    edge2->prevInSEL = edge1;
    edge2->nextInSEL = next;
  }
  else
  {
    TEdge* next = edge1->nextInSEL;
    TEdge* prev = edge1->prevInSEL;
    edge1->nextInSEL = edge2->nextInSEL;
    if( edge1->nextInSEL ) edge1->nextInSEL->prevInSEL = edge1;
    edge1->prevInSEL = edge2->prevInSEL;
    if( edge1->prevInSEL ) edge1->prevInSEL->nextInSEL = edge1;
    edge2->nextInSEL = next;
    if( edge2->nextInSEL ) edge2->nextInSEL->prevInSEL = edge2;
    edge2->prevInSEL = prev;
    if( edge2->prevInSEL ) edge2->prevInSEL->nextInSEL = edge2;
  }

  if( !edge1->prevInSEL ) m_SortedEdges = edge1;
  else if( !edge2->prevInSEL ) m_SortedEdges = edge2;
}
//------------------------------------------------------------------------------

TEdge *GetNextInAEL(TEdge *e, TDirection Direction)
{
  if( Direction == dLeftToRight ) return e->nextInAEL;
  else return e->prevInAEL;
}
//------------------------------------------------------------------------------

TEdge *GetPrevInAEL(TEdge *e, TDirection Direction)
{
  if( Direction == dLeftToRight ) return e->prevInAEL;
  else return e->nextInAEL;
}
//------------------------------------------------------------------------------

bool IsMinima(TEdge *e)
{
  return e  && (e->prev->nextInLML != e) && (e->next->nextInLML != e);
}
//------------------------------------------------------------------------------

bool IsMaxima(TEdge *e, const double &Y)
{
  return e  && std::fabs(e->ytop - Y) < tolerance &&  !e->nextInLML;
}
//------------------------------------------------------------------------------

bool IsIntermediate(TEdge *e, const double &Y)
{
  return std::fabs( e->ytop - Y ) < tolerance && e->nextInLML;
}
//------------------------------------------------------------------------------

TEdge *GetMaximaPair(TEdge *e)
{
  if( !IsMaxima(e->next, e->ytop) || (e->next->xtop != e->xtop) )
    return e->prev; else
    return e->next;
}
//------------------------------------------------------------------------------

void Clipper::DoMaxima(TEdge *e, const double &topY)
{
  TEdge* eMaxPair = GetMaximaPair(e);
  double X = e->xtop;
  TEdge* eNext = e->nextInAEL;
  while( eNext != eMaxPair )
  {
    if (!eNext) throw clipperException("DoMaxima error");
    IntersectEdges( e , eNext , DoublePoint( X , topY ), ipBoth );
    eNext = eNext->nextInAEL;
  }
  if(  ( e->outIdx < 0 ) && ( eMaxPair->outIdx < 0 ) )
  {
    DeleteFromAEL( e );
    DeleteFromAEL( eMaxPair );
  }
  else if(  ( e->outIdx >= 0 ) && ( eMaxPair->outIdx >= 0 ) )
  {
    IntersectEdges( e , eMaxPair , DoublePoint(X, topY), ipNone );
  }
  else throw clipperException("DoMaxima error");
}
//------------------------------------------------------------------------------

void Clipper::ProcessHorizontals()
{
  TEdge* horzEdge = m_SortedEdges;
  while( horzEdge )
  {
    DeleteFromSEL( horzEdge );
    ProcessHorizontal( horzEdge );
    horzEdge = m_SortedEdges;
  }
}
//------------------------------------------------------------------------------

bool Clipper::IsTopHorz(const double &XPos)
{
  TEdge* e = m_SortedEdges;
  while( e )
  {
    if(  ( XPos >= min(e->xcurr, e->xtop) ) &&
      ( XPos <= max(e->xcurr, e->xtop) ) ) return false;
    e = e->nextInSEL;
  }
  return true;
}
//------------------------------------------------------------------------------

void Clipper::ProcessHorizontal(TEdge *horzEdge)
{
  TDirection Direction;
  double horzLeft, horzRight;

  if( horzEdge->xcurr < horzEdge->xtop )
  {
    horzLeft = horzEdge->xcurr;
    horzRight = horzEdge->xtop;
    Direction = dLeftToRight;
  } else
  {
    horzLeft = horzEdge->xtop;
    horzRight = horzEdge->xcurr;
    Direction = dRightToLeft;
  }

  TEdge* eMaxPair;
  if( horzEdge->nextInLML ) eMaxPair = 0;
  else eMaxPair = GetMaximaPair(horzEdge);

  TEdge* e = GetNextInAEL( horzEdge , Direction );
  while( e )
  {
    TEdge* eNext = GetNextInAEL( e, Direction );
    if((e->xcurr >= horzLeft - tolerance) && (e->xcurr <= horzRight + tolerance))
    {
      //ok, so far it looks like we're still in range of the horizontal edge
      if ( std::fabs(e->xcurr - horzEdge->xtop) < tolerance && horzEdge->nextInLML)
      {
        if ( SlopesEqual(*e, *horzEdge->nextInLML) )
        {
          //we've got 2 colinear edges at the end of the horz. line ...
          if (horzEdge->outIdx >= 0 && e->outIdx >= 0)
          {
            int i = m_Joins.size();
            m_Joins.resize(i+1);
            TDoublePoint pt = DoublePoint(horzEdge->xtop, horzEdge->ytop);
            AddPolyPt(horzEdge, pt);
            AddPolyPt(e, pt);
            m_Joins[i].idx1 = horzEdge->outIdx;
            m_Joins[i].idx2 = e->outIdx;
            m_Joins[i].pt = pt;
          }
          break; //we've reached the end of the horizontal line
        }
        else if (e->dx < horzEdge->nextInLML->dx) break;
      }

      if( e == eMaxPair )
      {
        //horzEdge is evidently a maxima horizontal and we've arrived at its end.
        if (Direction == dLeftToRight)
          IntersectEdges(horzEdge, e, DoublePoint(e->xcurr, horzEdge->ycurr), ipNone);
        else
          IntersectEdges(e, horzEdge, DoublePoint(e->xcurr, horzEdge->ycurr), ipNone);
        return;
      }
      else if( IsHorizontal(*e) &&  !IsMinima(e) &&  !(e->xcurr > e->xtop) )
      {
        if(  Direction == dLeftToRight )
          IntersectEdges( horzEdge , e , DoublePoint(e->xcurr, horzEdge->ycurr),
            (IsTopHorz( e->xcurr ))? ipLeft : ipBoth );
        else
          IntersectEdges( e , horzEdge , DoublePoint(e->xcurr, horzEdge->ycurr),
            (IsTopHorz( e->xcurr ))? ipRight : ipBoth );
      }
      else if( Direction == dLeftToRight )
      {
        IntersectEdges( horzEdge , e , DoublePoint(e->xcurr, horzEdge->ycurr),
          (IsTopHorz( e->xcurr ))? ipLeft : ipBoth );
      }
      else
      {
        IntersectEdges( e , horzEdge , DoublePoint(e->xcurr, horzEdge->ycurr),
          (IsTopHorz( e->xcurr ))? ipRight : ipBoth );
      }
      SwapPositionsInAEL( horzEdge , e );
    }
    else if(  ( Direction == dLeftToRight ) &&
      ( e->xcurr > horzRight + tolerance ) &&  !horzEdge->nextInSEL ) break;
    else if(  ( Direction == dRightToLeft ) &&
      ( e->xcurr < horzLeft - tolerance ) &&  !horzEdge->nextInSEL  ) break;
    e = eNext;
  } //end while ( e )

  if( horzEdge->nextInLML )
  {
    if( horzEdge->outIdx >= 0 )
      AddPolyPt( horzEdge, DoublePoint(horzEdge->xtop, horzEdge->ytop));
    UpdateEdgeIntoAEL( horzEdge );
  }
  else
  {
    if ( horzEdge->outIdx >= 0 )
      IntersectEdges( horzEdge, eMaxPair,
        DoublePoint(horzEdge->xtop, horzEdge->ycurr), ipBoth);
    if (eMaxPair->outIdx >= 0) throw clipperException("ProcessHorizontal error");
    DeleteFromAEL(eMaxPair);
    DeleteFromAEL(horzEdge);
  }
}
//------------------------------------------------------------------------------

TPolyPt* Clipper::InsertPolyPtBetween(const TDoublePoint &pt, TPolyPt* pp1, TPolyPt* pp2)
{
  TPolyPt* pp = new TPolyPt;
  pp->pt = pt;
  if (pp2 == pp1->next)
  {
    pp->next = pp2;
    pp->prev = pp1;
    pp1->next = pp;
    pp2->prev = pp;
  }
  else if (pp1 == pp2->next)
  {
    pp->next = pp1;
    pp->prev = pp2;
    pp2->next = pp;
    pp1->prev = pp;
  }
  else
    throw clipperException("InsertPolyPtBetween error");
  return pp;
}
//------------------------------------------------------------------------------

TPolyPt* Clipper::AddPolyPt(TEdge *e, const TDoublePoint &pt)
{
  bool ToFront = (e->side == esLeft);
  if(  e->outIdx < 0 )
  {
    TPolyPt* newPolyPt = new TPolyPt;
    newPolyPt->pt = pt;
    m_PolyPts.push_back(newPolyPt);
    newPolyPt->next = newPolyPt;
    newPolyPt->prev = newPolyPt;
    newPolyPt->flags = (TOrientationFlag)0;
    newPolyPt->dx = unassigned;
    e->outIdx = m_PolyPts.size()-1;
    return newPolyPt;
  } else
  {
    TPolyPt* pp = m_PolyPts[e->outIdx];
    if (ToFront && PointsEqual(pt, pp->pt)) return pp;
    if (!ToFront && PointsEqual(pt, pp->prev->pt)) return pp->prev;
    TPolyPt* newPolyPt = new TPolyPt;
    newPolyPt->pt = pt;
    newPolyPt->next = pp;
    newPolyPt->prev = pp->prev;
    newPolyPt->prev->next = newPolyPt;
    pp->prev = newPolyPt;
    newPolyPt->flags = (TOrientationFlag)0;
    newPolyPt->dx = unassigned;
    if (ToFront) m_PolyPts[e->outIdx] = newPolyPt;
    return newPolyPt;
  }
}
//------------------------------------------------------------------------------

void Clipper::ProcessIntersections( const double &topY)
{
  if( !m_ActiveEdges ) return;
  try {
    m_IntersectTolerance = tolerance;
    BuildIntersectList( topY );
    if (!m_IntersectNodes) return;
    //Test pending intersections for errors and, if any are found, redo
    //BuildIntersectList (twice if necessary) with adjusted tolerances.
    //While this adds ~2% extra to processing time, I believe this is justified
    //by further halving of the algorithm's failure rate, though admittedly
    //failures were already extremely rare ...
    if ( !TestIntersections() )
    {
      m_IntersectTolerance = minimal_tolerance;
      DisposeIntersectNodes();
      BuildIntersectList( topY );
      if ( !TestIntersections() )
      {
        m_IntersectTolerance = slope_precision;
        DisposeIntersectNodes();
        BuildIntersectList( topY );
        if (!TestIntersections())
          //try eliminating near duplicate points in the input polygons
          //eg by adjusting precision ... to say 0.1;
          throw clipperException("Intersection error");
      }
    }
    ProcessIntersectList();
  }
  catch(...) {
    m_SortedEdges = 0;
    DisposeIntersectNodes();
    throw clipperException("ProcessIntersections error");
  }
}
//------------------------------------------------------------------------------

void Clipper::DisposeIntersectNodes()
{
  while ( m_IntersectNodes )
  {
    TIntersectNode* iNode = m_IntersectNodes->next;
    delete m_IntersectNodes;
    m_IntersectNodes = iNode;
  }
}
//------------------------------------------------------------------------------

bool E1PrecedesE2inAEL(TEdge *e1, TEdge *e2)
{
  while( e1 ){
    if(  e1 == e2 ) return true;
    else e1 = e1->nextInAEL;
  }
  return false;
}
//------------------------------------------------------------------------------

bool Clipper::Process1Before2(TIntersectNode *Node1, TIntersectNode *Node2)
{
  if ( std::fabs(Node1->pt.Y - Node2->pt.Y) < m_IntersectTolerance )
  {
    if ( std::fabs(Node1->pt.X - Node2->pt.X) > precision )
      return Node1->pt.X < Node2->pt.X;
    //a complex intersection (with more than 2 edges intersecting) ...
    if ( Node1->edge1 == Node2->edge1  || SlopesEqual(*Node1->edge1, *Node2->edge1) )
    {
      if (Node1->edge2 == Node2->edge2 )
        //(N1.E1 & N2.E1 are co-linear) and (N1.E2 == N2.E2)  ...
        return !E1PrecedesE2inAEL(Node1->edge1, Node2->edge1);
      else if ( SlopesEqual(*Node1->edge2, *Node2->edge2) )
        //(N1.E1 == N2.E1) and (N1.E2 & N2.E2 are co-linear) ...
        return E1PrecedesE2inAEL(Node1->edge2, Node2->edge2);
      else if //check if minima **
        ( (std::fabs(Node1->edge2->ybot - Node1->pt.Y) < slope_precision  ||
        std::fabs(Node2->edge2->ybot - Node2->pt.Y) < slope_precision ) &&
        (Node1->edge2->next == Node2->edge2 || Node1->edge2->prev == Node2->edge2) )
      {
        if ( Node1->edge1->dx < 0 ) return Node1->edge2->dx > Node2->edge2->dx;
        else return Node1->edge2->dx < Node2->edge2->dx;
      }
      else if ( (Node1->edge2->dx - Node2->edge2->dx) < precision )
        return E1PrecedesE2inAEL(Node1->edge2, Node2->edge2);
      else
        return (Node1->edge2->dx < Node2->edge2->dx);

    } else if ( Node1->edge2 == Node2->edge2  && //check if maxima ***
      (std::fabs(Node1->edge1->ytop - Node1->pt.Y) < slope_precision ||
      std::fabs(Node2->edge1->ytop - Node2->pt.Y) < slope_precision) )
        return (Node1->edge1->dx > Node2->edge1->dx);
    else
      return (Node1->edge1->dx < Node2->edge1->dx);
  } else
      return (Node1->pt.Y > Node2->pt.Y);
  //**a minima that very slightly overlaps an edge can appear like
  //a complex intersection but it's not. (Minima can't have parallel edges.)
  //***a maxima that very slightly overlaps an edge can appear like
  //a complex intersection but it's not. (Maxima can't have parallel edges.)
}
//------------------------------------------------------------------------------

void Clipper::AddIntersectNode(TEdge *e1, TEdge *e2, const TDoublePoint &pt)
{
  TIntersectNode* IntersectNode = new TIntersectNode;
  IntersectNode->edge1 = e1;
  IntersectNode->edge2 = e2;
  IntersectNode->pt = pt;
  IntersectNode->next = 0;
  IntersectNode->prev = 0;
  if( !m_IntersectNodes )
    m_IntersectNodes = IntersectNode;
  else if(  Process1Before2(IntersectNode , m_IntersectNodes) )
  {
    IntersectNode->next = m_IntersectNodes;
    m_IntersectNodes->prev = IntersectNode;
    m_IntersectNodes = IntersectNode;
  }
  else
  {
    TIntersectNode* iNode = m_IntersectNodes;
    while( iNode->next  && Process1Before2(iNode->next, IntersectNode) )
        iNode = iNode->next;
    if( iNode->next ) iNode->next->prev = IntersectNode;
    IntersectNode->next = iNode->next;
    IntersectNode->prev = iNode;
    iNode->next = IntersectNode;
  }
}
//------------------------------------------------------------------------------

void Clipper::BuildIntersectList( const double &topY)
{
  //prepare for sorting ...
  TEdge* e = m_ActiveEdges;
  e->tmpX = TopX( e, topY );
  m_SortedEdges = e;
  m_SortedEdges->prevInSEL = 0;
  e = e->nextInAEL;
  while( e )
  {
    e->prevInSEL = e->prevInAEL;
    e->prevInSEL->nextInSEL = e;
    e->nextInSEL = 0;
    e->tmpX = TopX( e, topY );
    e = e->nextInAEL;
  }

  //bubblesort ...
  bool isModified = true;
  while( isModified && m_SortedEdges )
  {
    isModified = false;
    e = m_SortedEdges;
    while( e->nextInSEL )
    {
      TEdge *eNext = e->nextInSEL;
      TDoublePoint pt;
      if((e->tmpX > eNext->tmpX + tolerance) && IntersectPoint(*e, *eNext, pt))
      {
        AddIntersectNode( e, eNext, pt );
        SwapPositionsInSEL(e, eNext);
        isModified = true;
      }
      else
        e = eNext;
    }
    if( e->prevInSEL ) e->prevInSEL->nextInSEL = 0;
    else break;
  }
  m_SortedEdges = 0;
}
//------------------------------------------------------------------------------

bool Clipper::TestIntersections()
{
  if ( !m_IntersectNodes ) return true;
  //do the test sort using SEL ...
  CopyAELToSEL();
  TIntersectNode* iNode = m_IntersectNodes;
  while ( iNode )
  {
    SwapPositionsInSEL(iNode->edge1, iNode->edge2);
    iNode = iNode->next;
  }
  //now check that tmpXs are in the right order ...
  TEdge* e = m_SortedEdges;
  while ( e->nextInSEL )
  {
    if ( e->nextInSEL->tmpX < e->tmpX - precision ) return false;
    e = e->nextInSEL;
  }
  m_SortedEdges = 0;
  return true;
}
//------------------------------------------------------------------------------

void Clipper::ProcessIntersectList()
{
  while( m_IntersectNodes )
  {
    TIntersectNode* iNode = m_IntersectNodes->next;
    {
      IntersectEdges( m_IntersectNodes->edge1 ,
        m_IntersectNodes->edge2 , m_IntersectNodes->pt, ipBoth );
      SwapPositionsInAEL( m_IntersectNodes->edge1 , m_IntersectNodes->edge2 );
    }
    delete m_IntersectNodes;
    m_IntersectNodes = iNode;
  }
}
//------------------------------------------------------------------------------

void Clipper::DoEdge1(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt)
{
  AddPolyPt(edge1, pt);
  SwapSides(*edge1, *edge2);
  SwapPolyIndexes(*edge1, *edge2);
}
//----------------------------------------------------------------------

void Clipper::DoEdge2(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt)
{
  AddPolyPt(edge2, pt);
  SwapSides(*edge1, *edge2);
  SwapPolyIndexes(*edge1, *edge2);
}
//----------------------------------------------------------------------

void Clipper::DoBothEdges(TEdge *edge1, TEdge *edge2, const TDoublePoint &pt)
{
  AddPolyPt(edge1, pt);
  AddPolyPt(edge2, pt);
  SwapSides( *edge1 , *edge2 );
  SwapPolyIndexes( *edge1 , *edge2 );
}
//----------------------------------------------------------------------

void Clipper::IntersectEdges(TEdge *e1, TEdge *e2,
     const TDoublePoint &pt, TIntersectProtects protects)
{
  //e1 will be to the left of e2 BELOW the intersection. Therefore e1 is before
  //e2 in AEL except when e1 is being inserted at the intersection point ...

  bool e1stops = !(ipLeft & protects) &&  !e1->nextInLML &&
    ( std::fabs( e1->xtop - pt.X ) < tolerance ) && //nb: not precision
    ( std::fabs( e1->ytop - pt.Y ) < tolerance );
  bool e2stops = !(ipRight & protects) &&  !e2->nextInLML &&
    ( std::fabs( e2->xtop - pt.X ) < tolerance ) && //nb: not precision
    ( std::fabs( e2->ytop - pt.Y ) < tolerance );
  bool e1Contributing = ( e1->outIdx >= 0 );
  bool e2contributing = ( e2->outIdx >= 0 );

  //update winding counts...
  //assumes that e1 will be to the right of e2 ABOVE the intersection
  if ( e1->polyType == e2->polyType )
  {
    if ( IsNonZeroFillType( *e1) )
    {
      if (e1->windCnt + e2->windDelta == 0 ) e1->windCnt = -e1->windCnt;
      else e1->windCnt += e2->windDelta;
      if ( e2->windCnt - e1->windDelta == 0 ) e2->windCnt = -e2->windCnt;
      else e2->windCnt -= e1->windDelta;
    } else
    {
      int oldE1WindCnt = e1->windCnt;
      e1->windCnt = e2->windCnt;
      e2->windCnt = oldE1WindCnt;
    }
  } else
  {
    if ( IsNonZeroFillType(*e2) ) e1->windCnt2 += e2->windDelta;
    else e1->windCnt2 = ( e1->windCnt2 == 0 ) ? 1 : 0;
    if ( IsNonZeroFillType(*e1) ) e2->windCnt2 -= e1->windDelta;
    else e2->windCnt2 = ( e2->windCnt2 == 0 ) ? 1 : 0;
  }

  if ( e1Contributing && e2contributing )
  {
    if ( e1stops || e2stops || std::abs(e1->windCnt) > 1 ||
      std::abs(e2->windCnt) > 1 ||
      (e1->polyType != e2->polyType && m_ClipType != ctXor) )
        AddLocalMaxPoly(e1, e2, pt); else
        DoBothEdges( e1, e2, pt );
  }
  else if ( e1Contributing )
  {
    switch( m_ClipType ) {
      case ctIntersection:
        if ( (e2->polyType == ptSubject || e2->windCnt2 != 0) &&
           std::abs(e2->windCnt) < 2 ) DoEdge1( e1, e2, pt);
        break;
      default:
        if ( std::abs(e2->windCnt) < 2 ) DoEdge1(e1, e2, pt);
    }
  }
  else if ( e2contributing )
  {
    switch( m_ClipType ) {
      case ctIntersection:
        if ( (e1->polyType == ptSubject || e1->windCnt2 != 0) &&
          std::abs(e1->windCnt) < 2 ) DoEdge2( e1, e2, pt );
        break;
      default:
        if (std::abs(e1->windCnt) < 2) DoEdge2( e1, e2, pt );
    }
  } else
  {
    //neither edge is currently contributing ...
    if ( std::abs(e1->windCnt) > 1 && std::abs(e2->windCnt) > 1 ) ;// do nothing
    else if ( e1->polyType != e2->polyType && !e1stops && !e2stops &&
      std::abs(e1->windCnt) < 2 && std::abs(e2->windCnt) < 2 )
        AddLocalMinPoly(e1, e2, pt);
    else if ( std::abs(e1->windCnt) == 1 && std::abs(e2->windCnt) == 1 )
      switch( m_ClipType ) {
        case ctIntersection:
          if ( std::abs(e1->windCnt2) > 0 && std::abs(e2->windCnt2) > 0 )
            AddLocalMinPoly(e1, e2, pt);
          break;
        case ctUnion:
          if ( e1->windCnt2 == 0 && e2->windCnt2 == 0 )
            AddLocalMinPoly(e1, e2, pt);
          break;
        case ctDifference:
          if ( (e1->polyType == ptClip && e2->polyType == ptClip &&
            e1->windCnt2 != 0 && e2->windCnt2 != 0) ||
            (e1->polyType == ptSubject && e2->polyType == ptSubject &&
            e1->windCnt2 == 0 && e2->windCnt2 == 0) )
              AddLocalMinPoly(e1, e2, pt);
          break;
        case ctXor:
          AddLocalMinPoly(e1, e2, pt);
      }
    else if ( std::abs(e1->windCnt) < 2 && std::abs(e2->windCnt) < 2 )
      SwapSides( *e1, *e2 );
  }

  if(  (e1stops != e2stops) &&
    ( (e1stops && (e1->outIdx >= 0)) || (e2stops && (e2->outIdx >= 0)) ) )
  {
    SwapSides( *e1, *e2 );
    SwapPolyIndexes( *e1, *e2 );
  }

  //finally, delete any non-contributing maxima edges  ...
  if( e1stops ) DeleteFromAEL( e1 );
  if( e2stops ) DeleteFromAEL( e2 );
}
//------------------------------------------------------------------------------

void Clipper::DeleteFromAEL(TEdge *e)
{
  TEdge* AelPrev = e->prevInAEL;
  TEdge* AelNext = e->nextInAEL;
  if(  !AelPrev &&  !AelNext && (e != m_ActiveEdges) ) return; //already deleted
  if( AelPrev ) AelPrev->nextInAEL = AelNext;
  else m_ActiveEdges = AelNext;
  if( AelNext ) AelNext->prevInAEL = AelPrev;
  e->nextInAEL = 0;
  e->prevInAEL = 0;
}
//------------------------------------------------------------------------------

void Clipper::DeleteFromSEL(TEdge *e)
{
  TEdge* SelPrev = e->prevInSEL;
  TEdge* SelNext = e->nextInSEL;
  if(  !SelPrev &&  !SelNext && (e != m_SortedEdges) ) return; //already deleted
  if( SelPrev ) SelPrev->nextInSEL = SelNext;
  else m_SortedEdges = SelNext;
  if( SelNext ) SelNext->prevInSEL = SelPrev;
  e->nextInSEL = 0;
  e->prevInSEL = 0;
}
//------------------------------------------------------------------------------

void Clipper::UpdateEdgeIntoAEL(TEdge *&e)
{
  if( !e->nextInLML ) throw
    clipperException("UpdateEdgeIntoAEL: invalid call");
  TEdge* AelPrev = e->prevInAEL;
  TEdge* AelNext = e->nextInAEL;
  e->nextInLML->outIdx = e->outIdx;
  if( AelPrev ) AelPrev->nextInAEL = e->nextInLML;
  else m_ActiveEdges = e->nextInLML;
  if( AelNext ) AelNext->prevInAEL = e->nextInLML;
  e->nextInLML->side = e->side;
  e->nextInLML->windDelta = e->windDelta;
  e->nextInLML->windCnt = e->windCnt;
  e->nextInLML->windCnt2 = e->windCnt2;
  e = e->nextInLML;
  e->prevInAEL = AelPrev;
  e->nextInAEL = AelNext;
  if( !IsHorizontal(*e) )
  {
    InsertScanbeam( e->ytop );

    //if output polygons share an edge, they'll need joining later ...
    if (e->outIdx >= 0 && AelPrev && AelPrev->outIdx >= 0 &&
      std::fabs(AelPrev->xcurr - e->xbot) < tolerance && SlopesEqual(*e, *AelPrev))
    {
      int i = m_Joins.size();
      m_Joins.resize(i+1);
      TDoublePoint pt = DoublePoint(e->xbot, e->ybot);
      AddPolyPt(AelPrev, pt);
      AddPolyPt(e, pt);
      m_Joins[i].idx1 = AelPrev->outIdx;
      m_Joins[i].idx2 = e->outIdx;
      m_Joins[i].pt = pt;
    }
  }
}
//------------------------------------------------------------------------------

bool Clipper::IsContributing(const TEdge& edge) const
{
  switch( m_ClipType ){
    case ctIntersection:
      if ( edge.polyType == ptSubject )
        return std::abs(edge.windCnt) == 1 && edge.windCnt2 != 0; else
        return std::abs(edge.windCnt2) > 0 && std::abs(edge.windCnt) == 1;
    case ctUnion:
      return std::abs(edge.windCnt) == 1 && edge.windCnt2 == 0;
    case ctDifference:
      if ( edge.polyType == ptSubject )
        return std::abs(edge.windCnt) == 1 && edge.windCnt2 == 0; else
        return std::abs(edge.windCnt) == 1 && edge.windCnt2 != 0;
    default: //case ctXor:
      return std::abs(edge.windCnt) == 1;
  }
}
//------------------------------------------------------------------------------

bool Clipper::Execute(TClipType clipType, TPolyPolygon &solution,
    TPolyFillType subjFillType, TPolyFillType clipFillType)
{
  m_SubjFillType = subjFillType;
  m_ClipFillType = clipFillType;

  bool succeeded = false;
  solution.resize(0);
  if(  m_ExecuteLocked || !InitializeScanbeam() ) return false;
  try {
    m_ExecuteLocked = true;
    m_ActiveEdges = 0;
    m_SortedEdges = 0;
    m_ClipType = clipType;
    m_Joins.clear();
    m_CurrentHorizontals.clear();

    double botY = PopScanbeam();
    do {
      InsertLocalMinimaIntoAEL( botY );
      ProcessHorizontals();
      double topY = PopScanbeam();
      ProcessIntersections( topY );
      ProcessEdgesAtTopOfScanbeam( topY );
      botY = topY;
    } while( m_Scanbeam );

    //build the return polygons ...
    BuildResult(solution);
    succeeded = true;
  }
  catch(...) {
    solution.resize(0);
    //returns false ...
  }
  DisposeAllPolyPts();
  m_Joins.clear();
  m_ExecuteLocked = false;
  return succeeded;
}
//------------------------------------------------------------------------------

TPolyPt* FixupOutPolygon(TPolyPt *p, bool stripPointyEdgesOnly = false)
{
  //FixupOutPolygon() - removes duplicate points and simplifies consecutive
  //parallel edges by removing the middle vertex.
  //stripPointyEdgesOnly: removes the middle vertex only when consecutive
  //parallel edges reflect back on themselves ('pointy' edges). However, it
  //doesn't remove the middle vertex when edges are parallel continuations.
  //Given 3 consecutive vertices - o, *, and o ...
  //the form of 'non-pointy' parallel edges is : o--*----------o
  //the form of 'pointy' parallel edges is     : o--o----------*
  //(While merging polygons that share common edges, it's necessary to
  //temporarily retain 'non-pointy' parallel edges.)
  if (!p) return 0;
  TPolyPt *pp = p, *result = p, *lastOK = 0;
  for (;;)
  {
    if (pp->prev == pp || pp->prev == pp->next )
    {
      DisposePolyPts(pp);
      return 0;
    }
    //test for duplicate points and for same slope (cross-product) ...
    if (PointsEqual(pp->pt, pp->next->pt) ||
      (SlopesEqual(pp->prev->pt, pp->pt, pp->next->pt) &&
      (!stripPointyEdgesOnly ||
      ((pp->pt.X - pp->prev->pt.X > 0) != (pp->next->pt.X - pp->pt.X > 0)) ||
      ((pp->pt.Y - pp->prev->pt.Y > 0) != (pp->next->pt.Y - pp->pt.Y > 0)))))
    {
      lastOK = 0;
      pp->prev->next = pp->next;
      pp->next->prev = pp->prev;
      TPolyPt* tmp = pp;
      if (pp == result) result = pp->prev;
      pp = pp->prev;
      delete tmp;
    }
    else if (pp == lastOK) break;
    else
    {
      if (!lastOK) lastOK = pp;
      pp = pp->next;
    }
  }
  return result;
}
//------------------------------------------------------------------------------

TPolyPt* FixupOutPolygon2(TPolyPt *p, bool stripPointyEdgesOnly = false)
{
  //FixupOutPolygon2 - just removes duplicate points ...
  if (!p) return 0;
  TPolyPt *pp = p, *result = p, *lastOK = 0;
  for (;;)
  {
    if (pp->prev == pp || pp->prev == pp->next )
    {
      DisposePolyPts(pp);
      return 0;
    }
    //test for duplicate points and for same slope (cross-product) ...
    if (PointsEqual(pp->pt, pp->next->pt))
    {
      lastOK = 0;
      pp->prev->next = pp->next;
      pp->next->prev = pp->prev;
      TPolyPt* tmp = pp;
      if (pp == result) result = pp->prev;
      pp = pp->prev;
      delete tmp;
    }
    else if (pp == lastOK) break;
    else
    {
      if (!lastOK) lastOK = pp;
      pp = pp->next;
    }
  }
  return result;
}
//------------------------------------------------------------------------------

int sCompare(TPolyPt* item1, TPolyPt* item2)
{
  return 0;
}
//------------------------------------------------------------------------------

void Clipper::BuildResult(TPolyPolygon &polypoly){
  MergePolysWithCommonEdges();
  for (PolyPtList::size_type i = 0; i < m_PolyPts.size(); ++i)
    if (m_PolyPts[i])
      m_PolyPts[i] = FixupOutPolygon(m_PolyPts[i]);

  if (!m_IgnoreOrientation) FixOrientation();

  int k = 0;
  polypoly.resize(m_PolyPts.size());
  for (unsigned i = 0; i < m_PolyPts.size(); ++i) {
    if (m_PolyPts[i]) {
      TPolygon* pg = &polypoly[k];
      pg->clear();
      TPolyPt* p = m_PolyPts[i];
      do {
        pg->push_back(p->pt);
        p = p->next;
      } while (p != m_PolyPts[i]);

      //nb: for those who might want to modify the TPolyPolygon struct to
      //include orientation, p->flags will have the ofClockwise bit set when
      //it's an outer polygon.

      if (pg->size() < 3) pg->clear(); else k++;
    }
  }

}
//------------------------------------------------------------------------------

bool Clipper::IgnoreOrientation(){
  return m_IgnoreOrientation;
}
//------------------------------------------------------------------------------

void Clipper::IgnoreOrientation(bool value){
  m_IgnoreOrientation = value;
}
//------------------------------------------------------------------------------

TEdge* Clipper::BubbleSwap(TEdge *edge)
{
  int n = 1;
  TEdge* result = edge->nextInAEL;
  while( result  && ( std::fabs(result->xcurr - edge->xcurr) <= tolerance ) )
  {
    ++n;
    result = result->nextInAEL;
  }
  //if more than 2 edges intersect at a given point then there are multiple
  //intersections at this point between all the edges. Therefore ...
  //let n = no. edges intersecting at a given point.
  //given f(n) = no. intersections between n edges at a given point, & f(0) = 0
  //then f(n) = f(n-1) + n-1;
  //therefore 1 edge -> 0 intersections; 2 -> 1; 3 -> 3; 4 -> 6; 5 -> 10 etc.
  //nb: coincident edges will cause unexpected f(n) values.
  if( n > 2 )
  {
    //create the sort list ...
    try {
      m_SortedEdges = edge;
      edge->prevInSEL = 0;
      TEdge *e = edge->nextInAEL;
      for( int i = 2 ; i <= n ; ++i )
      {
        e->prevInSEL = e->prevInAEL;
        e->prevInSEL->nextInSEL = e;
        if(  i == n ) e->nextInSEL = 0;
        e = e->nextInAEL;
      }
      while( m_SortedEdges  && m_SortedEdges->nextInSEL )
      {
        e = m_SortedEdges;
        while( e->nextInSEL )
        {
          if( e->nextInSEL->dx > e->dx )
          {
            IntersectEdges(e, e->nextInSEL,  //param order important here
              DoublePoint(e->xcurr, e->ycurr), ipBoth );
            SwapPositionsInAEL( e , e->nextInSEL );
            SwapPositionsInSEL( e , e->nextInSEL );
          }
          else
            e = e->nextInSEL;
        }
        e->prevInSEL->nextInSEL = 0; //removes 'e' from SEL
      }
    }
    catch(...) {
      m_SortedEdges = 0;
      throw clipperException("BubbleSwap error");
    }
    m_SortedEdges = 0;
  }
return result;
}
//------------------------------------------------------------------------------

void Clipper::ProcessEdgesAtTopOfScanbeam( const double &topY)
{
  TEdge* e = m_ActiveEdges;
  while( e )
  {
    //1. process maxima, treating them as if they're 'bent' horizontal edges,
    //   but exclude maxima with horizontal edges. nb: e can't be a horizontal.
    if( IsMaxima(e, topY) && !IsHorizontal(*GetMaximaPair(e)) )
    {
      //'e' might be removed from AEL, as may any following edges so ...
      TEdge* ePrior = e->prevInAEL;
      DoMaxima( e , topY );
      if( !ePrior ) e = m_ActiveEdges;
      else e = ePrior->nextInAEL;
    }
    else
    {
      //2. promote horizontal edges, otherwise update xcurr and ycurr ...
      if(  IsIntermediate( e , topY ) && IsHorizontal( *e->nextInLML ) )
      {
        if (e->outIdx >= 0)
        {
          TPolyPt* pp = AddPolyPt(e, DoublePoint(e->xtop, e->ytop));
          //add the polyPt to a list that later checks for overlaps with
          //contributing horizontal minima since they'll need joining...
          int i = m_CurrentHorizontals.size();
          m_CurrentHorizontals.resize(i+1);
          m_CurrentHorizontals[i].idx1 = e->outIdx;
          m_CurrentHorizontals[i].pt =
            DoublePoint(e->nextInLML->xtop, e->nextInLML->ytop);
          m_CurrentHorizontals[i].outPPt = pp;
        }
        //very rarely an edge just below a horizontal edge in a contour
        //intersects with another edge at the very top of a scanbeam.
        //If this happens that intersection must be managed first ...
        if ( e->prevInAEL && e->prevInAEL->xcurr > e->xtop + tolerance )
        {
          IntersectEdges(e->prevInAEL, e, DoublePoint(e->prevInAEL->xcurr,
            e->prevInAEL->ycurr), ipBoth);
          SwapPositionsInAEL(e->prevInAEL, e);
          UpdateEdgeIntoAEL(e);
          AddEdgeToSEL(e);
          e = e->nextInAEL;
          UpdateEdgeIntoAEL(e);
          AddEdgeToSEL(e);
        }
        else if (e->nextInAEL && e->xtop > TopX(e->nextInAEL, topY) + tolerance)
        {
          e->nextInAEL->xcurr = TopX(e->nextInAEL, topY);
          e->nextInAEL->ycurr = topY;
          IntersectEdges(e, e->nextInAEL, DoublePoint(e->nextInAEL->xcurr,
            e->nextInAEL->ycurr), ipBoth);
          SwapPositionsInAEL(e, e->nextInAEL);
          UpdateEdgeIntoAEL(e);
          AddEdgeToSEL(e);
        } else
        {
          UpdateEdgeIntoAEL(e);
          AddEdgeToSEL(e);
        }
      } else
      {
        //this just simplifies horizontal processing ...
        e->xcurr = TopX( e , topY );
        e->ycurr = topY;
      }
      e = e->nextInAEL;
    }
  }

  //3. Process horizontals at the top of the scanbeam ...
  ProcessHorizontals();

  //4. Promote intermediate vertices ...
  e = m_ActiveEdges;
  while( e )
  {
    if( IsIntermediate( e, topY ) )
    {
      if( e->outIdx >= 0 ) AddPolyPt(e, DoublePoint(e->xtop,e->ytop));
      UpdateEdgeIntoAEL(e);
    }
    e = e->nextInAEL;
  }

  //5. Process (non-horizontal) intersections at the top of the scanbeam ...
  e = m_ActiveEdges;
  if (e && !e->nextInAEL)
    throw clipperException("ProcessEdgesAtTopOfScanbeam() error");
  while( e )
  {
    if( !e->nextInAEL ) break;
    if( e->nextInAEL->xcurr < e->xcurr - precision )
      throw clipperException("ProcessEdgesAtTopOfScanbeam() error");
    if( e->nextInAEL->xcurr > e->xcurr + tolerance )
      e = e->nextInAEL;
    else
      e = BubbleSwap( e );
  }
}
//------------------------------------------------------------------------------

void Clipper::AddLocalMaxPoly(TEdge *e1, TEdge *e2, const TDoublePoint &pt)
{
  AddPolyPt( e1, pt );
  if(  EdgesShareSamePoly(*e1, *e2) )
  {
    e1->outIdx = -1;
    e2->outIdx = -1;
  }
  else AppendPolygon( e1, e2 );
}
//------------------------------------------------------------------------------

void Clipper::AddLocalMinPoly(TEdge *e1, TEdge *e2, const TDoublePoint &pt)
{
  AddPolyPt( e1, pt );
  e2->outIdx = e1->outIdx;

  if( IsHorizontal( *e2 ) || ( e1->dx > e2->dx ) )
  {
    e1->side = esLeft;
    e2->side = esRight;
  } else
  {
    e1->side = esRight;
    e2->side = esLeft;
  }
}
//------------------------------------------------------------------------------

void Clipper::AppendPolygon(TEdge *e1, TEdge *e2)
{
  //get the start and ends of both output polygons ...
  TPolyPt* p1_lft = m_PolyPts[e1->outIdx];
  TPolyPt* p1_rt = p1_lft->prev;
  TPolyPt* p2_lft = m_PolyPts[e2->outIdx];
  TPolyPt* p2_rt = p2_lft->prev;
  TEdgeSide side;

  //join e2 poly onto e1 poly and delete pointers to e2 ...
  if(  e1->side == esLeft )
  {
    if(  e2->side == esLeft )
    {
      //z y x a b c
      ReversePolyPtLinks(*p2_lft);
      p2_lft->next = p1_lft;
      p1_lft->prev = p2_lft;
      p1_rt->next = p2_rt;
      p2_rt->prev = p1_rt;
      m_PolyPts[e1->outIdx] = p2_rt;
    } else
    {
      //x y z a b c
      p2_rt->next = p1_lft;
      p1_lft->prev = p2_rt;
      p2_lft->prev = p1_rt;
      p1_rt->next = p2_lft;
      m_PolyPts[e1->outIdx] = p2_lft;
    }
    side = esLeft;
  } else
  {
    if(  e2->side == esRight )
    {
      //a b c z y x
      ReversePolyPtLinks( *p2_lft );
      p1_rt->next = p2_rt;
      p2_rt->prev = p1_rt;
      p2_lft->next = p1_lft;
      p1_lft->prev = p2_lft;
    } else
    {
      //a b c x y z
      p1_rt->next = p2_lft;
      p2_lft->prev = p1_rt;
      p1_lft->prev = p2_rt;
      p2_rt->next = p1_lft;
    }
    side = esRight;
  }

  int OKIdx = e1->outIdx;
  int ObsoleteIdx = e2->outIdx;
  m_PolyPts[ObsoleteIdx] = 0;

  for( JoinList::size_type i = 0 ; i < m_Joins.size() ; ++i )
  {
    if (m_Joins[i].idx1 == ObsoleteIdx) m_Joins[i].idx1 = OKIdx;
    if (m_Joins[i].idx2 == ObsoleteIdx) m_Joins[i].idx2 = OKIdx;
  }
  for( JoinList::size_type i = 0 ; i < m_CurrentHorizontals.size() ; ++i )
    if (m_CurrentHorizontals[i].idx1 == ObsoleteIdx)
      m_CurrentHorizontals[i].idx1 = OKIdx;

  e1->outIdx = -1; //nb: safe because we only get here via AddLocalMaxPoly
  e2->outIdx = -1;

  TEdge* e = m_ActiveEdges;
  while( e )
  {
    if( e->
    outIdx == ObsoleteIdx )
    {
      e->outIdx = OKIdx;
      e->side = side;
      break;
    }
    e = e->nextInAEL;
  }
}
//------------------------------------------------------------------------------

bool SlopesEqual(const TDoublePoint& pt1a, const TDoublePoint& pt1b,
  const TDoublePoint& pt2a, const TDoublePoint& pt2b)
{
  return std::fabs((pt1b.Y - pt1a.Y) * (pt2b.X - pt2a.X) -
    (pt1b.X - pt1a.X) * (pt2b.Y - pt2a.Y)) < slope_precision;
}
//------------------------------------------------------------------------------

void Clipper::MergePolysWithCommonEdges()
{
  TPolyPt *p1, *p2;
  for (JoinList::size_type i = 0; i < m_Joins.size(); ++i)
  {
    if (m_Joins[i].idx1 == m_Joins[i].idx2)
    {
      p1 = m_PolyPts[m_Joins[i].idx1];
      p1 = FixupOutPolygon(p1, true);
      m_PolyPts[m_Joins[i].idx1] = p1;
      if (!PtIsAPolyPt(m_Joins[i].pt, p1)) continue;
      p2 = p1->next; //ie we don't want the same point as p1
      if (!PtIsAPolyPt(m_Joins[i].pt, p2) || p2 == p1) continue;
    } else
    {
      p1 = m_PolyPts[m_Joins[i].idx1];
      p1 = FixupOutPolygon(p1, true);
      m_PolyPts[m_Joins[i].idx1] = p1;
      //check that fJoins[i].pt is in the polygon and also update p1 so
      //that p1.pt == fJoins[i].pt ...
      if (!PtIsAPolyPt(m_Joins[i].pt, p1)) continue;

      p2 = m_PolyPts[m_Joins[i].idx2];
      p2 = FixupOutPolygon(p2, true);
      m_PolyPts[m_Joins[i].idx2] = p2;
      if (!PtIsAPolyPt(m_Joins[i].pt, p2)) continue;
    }

    if (((p1->next->pt.X > p1->pt.X && p2->next->pt.X > p2->pt.X) ||
      (p1->next->pt.X < p1->pt.X && p2->next->pt.X < p2->pt.X) ||
      (p1->next->pt.Y > p1->pt.Y && p2->next->pt.Y > p2->pt.Y) ||
      (p1->next->pt.Y < p1->pt.Y && p2->next->pt.Y < p2->pt.Y)) &&
      SlopesEqual(p1->pt, p1->next->pt, p2->pt, p2->next->pt))
    {
      if (m_Joins[i].idx1 == m_Joins[i].idx2) continue;
      TPolyPt* pp1 = DuplicatePolyPt(p1);
      TPolyPt* pp2 = DuplicatePolyPt(p2);
      ReversePolyPtLinks( *p2 );
      pp1->prev = pp2;
      pp2->next = pp1;
      p1->next = p2;
      p2->prev = p1;
    }
    else if (((p1->next->pt.X > p1->pt.X && p2->prev->pt.X > p2->pt.X) ||
      (p1->next->pt.X < p1->pt.X && p2->prev->pt.X < p2->pt.X) ||
      (p1->next->pt.Y > p1->pt.Y && p2->prev->pt.Y > p2->pt.Y) ||
      (p1->next->pt.Y < p1->pt.Y && p2->prev->pt.Y < p2->pt.Y)) &&
      SlopesEqual(p1->pt, p1->next->pt, p2->pt, p2->prev->pt))
    {
      TPolyPt* pp1 = DuplicatePolyPt(p1);
      TPolyPt* pp2 = DuplicatePolyPt(p2);
      p1->next = pp2;
      pp2->prev = p1;
      p2->next = pp1;
      pp1->prev = p2;
    }
    else if (((p1->prev->pt.X > p1->pt.X && p2->next->pt.X > p2->pt.X) ||
      (p1->prev->pt.X < p1->pt.X && p2->next->pt.X < p2->pt.X) ||
      (p1->prev->pt.Y > p1->pt.Y && p2->next->pt.Y > p2->pt.Y) ||
      (p1->prev->pt.Y < p1->pt.Y && p2->next->pt.Y < p2->pt.Y)) &&
      SlopesEqual(p1->pt, p1->prev->pt, p2->pt, p2->next->pt))
    {
      TPolyPt* pp1 = DuplicatePolyPt(p1);
      TPolyPt* pp2 = DuplicatePolyPt(p2);
      p1->next = pp2;
      pp2->prev = p1;
      pp1->prev = p2;
      p2->next = pp1;
    }
    else if (((p1->prev->pt.X > p1->pt.X && p2->prev->pt.X > p2->pt.X) ||
      (p1->prev->pt.X < p1->pt.X && p2->prev->pt.X < p2->pt.X) ||
      (p1->prev->pt.Y > p1->pt.Y && p2->prev->pt.Y > p2->pt.Y) ||
      (p1->prev->pt.Y < p1->pt.Y && p2->prev->pt.Y < p2->pt.Y)) &&
      SlopesEqual(p1->pt, p1->prev->pt, p2->pt, p2->prev->pt))
    {
      if (m_Joins[i].idx1 == m_Joins[i].idx2) continue;
      TPolyPt* pp1 = DuplicatePolyPt(p1);
      TPolyPt* pp2 = DuplicatePolyPt(p2);
      ReversePolyPtLinks(*pp2);
      pp1->prev = pp2;
      pp2->next = pp1;
      p1->next = p2;
      p2->prev = p1;
    }
    else
      continue;

    if (m_Joins[i].idx1 == m_Joins[i].idx2)
    {
      //When an edge join occurs within the same polygon, then
      //that polygon effectively splits into 2 polygons ...
      p1 = FixupOutPolygon(p1, true);
      m_PolyPts[m_Joins[i].idx1] = p1;
      p2 = FixupOutPolygon(p2, true);
      int newIdx = m_PolyPts.size();
      m_PolyPts.push_back(p2);
      for (JoinList::size_type j = i+1; j < m_Joins.size(); ++j)
      {
        if (m_Joins[j].idx1 == m_Joins[i].idx1 &&
          PtIsAPolyPt(m_Joins[j].pt, p2))
            m_Joins[j].idx1 = newIdx;
        if (m_Joins[j].idx2 == m_Joins[i].idx1 &&
          PtIsAPolyPt(m_Joins[j].pt, p2))
            m_Joins[j].idx1 = newIdx;
      }
    } else
    {
      //When 2 polygons are merged (joined), pointers referencing the
      //'deleted' polygon must now point to the 'merged' polygon ...
      m_PolyPts[m_Joins[i].idx2] = 0;
      for (JoinList::size_type j = i+1; j < m_Joins.size(); ++j)
      {
        if (m_Joins[j].idx1 == m_Joins[i].idx2)
          m_Joins[j].idx1 = m_Joins[i].idx1;
        if (m_Joins[j].idx2 == m_Joins[i].idx2)
          m_Joins[j].idx2 = m_Joins[i].idx1;
      }
    }
  }
}
//------------------------------------------------------------------------------

double SetDx(TPolyPt* pp)
{
  if (pp->dx == unassigned)
  {
    TPolyPt* pp2;
    if (ofForwardBound & pp->flags) pp2 = pp->next; else pp2 = pp->prev;
    double dx = std::fabs(pp->pt.X - pp2->pt.X);
    double dy = std::fabs(pp->pt.Y - pp2->pt.Y);
    if ((dx < 0.1 && dy *10 < dx) || dy < precision)
      pp->dx = horizontal; else
      pp->dx = (pp->pt.X - pp2->pt.X)/(pp->pt.Y - pp2->pt.Y);
  }
  return pp->dx;
}
//------------------------------------------------------------------------------

void NextPoint(TPolyPt*& p, bool goingForward)
{
  if (goingForward)
  {
    while (PointsEqual(p->pt, p->next->pt)) p = p->next;
    p = p->next;
  } else
  {
    while (PointsEqual(p->pt, p->prev->pt)) p = p->prev;
    p = p->prev;
  }
}
//------------------------------------------------------------------------------

double GetR(const TDoublePoint pt1,
  const TDoublePoint pt2, const TDoublePoint pt3)
{
  //this function is useful when COMPARING angles as it's a little quicker
  //than getting the specific angles using arctan().
  //Return value are between -2 and +2 where -1.99 is an acute angle turning
  //right, +1.99 is an acute angle turn left and 0 when the points are parallel.
  TDoublePoint N1 = GetUnitNormal(pt1, pt2);
  TDoublePoint N2 = GetUnitNormal(pt2, pt3);
  if (N1.X * N2.Y - N2.X * N1.Y < 0)
    return 1- (N1.X*N2.X + N1.Y*N2.Y); else
    return (N1.X*N2.X + N1.Y*N2.Y) -1;
}
//------------------------------------------------------------------------------

double DistanceSqr(const TDoublePoint pt1, const TDoublePoint pt2)
{
  return (pt1.X - pt2.X)*(pt1.X - pt2.X) + (pt1.Y - pt2.Y)*(pt1.Y - pt2.Y);
}
//------------------------------------------------------------------------------

int CompareForwardAngles(TPolyPt* p1, TPolyPt* p2)
{
  //preconditions:
  //1. p1a == p2a
  //2. p1->p1nextInBound is colinear with p2->p2nextInBound

  bool p1Forward = (ofForwardBound & p1->flags) != 0;
  bool p2Forward = (ofForwardBound & p2->flags) != 0;
  TPolyPt* pTmp = 0;
  TDoublePoint p1a, p1b, p1c,  p2a, p2b, p2c;
  p1b = p1c = p1->pt;
  p2b = p2c = p2->pt;
  do
  {
    p1a = p1b; p2a = p2b; p1b = p1c; p2b = p2c;
    NextPoint(p1, p1Forward);
    NextPoint(p2, p2Forward);

    //the following avoids a very rare endless loop where the
    //p1 & p2 polys are almost identical except for their orientations ...
    if (pTmp == 0) pTmp = p1;
    else if (pTmp == p1)
    {
      if (PointsEqual(p1c, p2c)) return 1;
      break;
    }
    p1c = p1->pt; p2c = p2->pt;
  }
  while (PointsEqual(p1c, p2c));

  

  //and now ... p1c != p2c ...
  if (PointsEqual(p1a, p1b) ||
    PointsEqual(GetUnitNormal(p1b, p1c),GetUnitNormal(p2b, p2c)))
  {
    //we have parallel edges of unequal length ...
    if (DistanceSqr(p1b, p1c) < DistanceSqr(p2b, p2c))
    {
      p1a = p1b; p1b = p1c;
      NextPoint(p1, p1Forward);
      double r1 = GetR(p1a, p1b, p1->pt);
      if (r1 > minimal_tolerance) return 1;					// minimal_tolerance
      else if (r1 < -minimal_tolerance) return -1;			// minimal_tolerance
      else throw clipperException("CompareForwardAngles error");
    } else
    {
      p2a = p2b; p2b = p2c;
      NextPoint(p2, p2Forward);
      double r2 = GetR(p2a, p2b, p2->pt);
      if (r2 > minimal_tolerance) return -1;				// minimal_tolerance
      else if (r2 < -minimal_tolerance) return 1;			// minimal_tolerance
      else throw clipperException("CompareForwardAngles error");
    }
  } else
  {
    double r1 = GetR(p1a, p1b, p1c);
    double r2 = GetR(p2a, p2b, p2c);
    if (r1 > r2 + minimal_tolerance) return 1;			// minimal_tolerance
    else if (r1 < r2 - minimal_tolerance) return -1;		// minimal_tolerance
    else throw clipperException("CompareForwardAngles error");
  }
}
//------------------------------------------------------------------------------

int blCompare(TPolyPt* pp1, TPolyPt* pp2)
{
  if (pp2->pt.Y > pp1->pt.Y + precision) return -1;
  else if (pp2->pt.Y < pp1->pt.Y - precision) return 1;
  else if (pp2->pt.X < pp1->pt.X - precision) return -1;
  else if (pp2->pt.X > pp1->pt.X + precision) return 1;
  else if (pp1 == pp2) return 0;
  else
  {
    double dx1 = SetDx(pp1);
    double dx2 = SetDx(pp2);
    if (dx1 < dx2 - precision) return -1;
    else if (dx1 > dx2 + precision) return 1;
    else return CompareForwardAngles(pp1, pp2);
  }
}
//------------------------------------------------------------------------------

int wlCompare(TPolyPt* pp1, TPolyPt* pp2)
{
  //nb: 1. when item1 < item2, result = 1; when item2 < item1, result = -1
  //    2. wlCompare is only ever used for insertions into the skiplist.
  //    3. item2 is always the item being inserted.
  TPolyPt *pp1Next, *pp2Next;
  if (ofForwardBound & pp1->flags)
    pp1Next = pp1->next; else
    pp1Next = pp1->prev;

  if (pp1 == pp2) return 0;
  else if (pp1->pt.X < pp2->pt.X - tolerance &&
    pp1Next->pt.X < pp2->pt.X + tolerance) return 1;
  else if (pp1->pt.X > pp2->pt.X + tolerance &&
    pp1Next->pt.X > pp2->pt.X - tolerance) return -1;
  else if (PointsEqual(pp1->pt, pp2->pt))
  {
    double dx1 = SetDx(pp1); double dx2 = SetDx(pp2);
    //dx1 should never be horizontal, but if dx2 is horizontal ...
    if (dx2 == horizontal)
    {
      if (ofForwardBound & pp2->flags)
        pp2Next = pp2->next; else pp2Next = pp2->prev;
      if (pp2Next->pt.X < pp1->pt.X) return -1; else return 1;
    }
    else if (dx1 < dx2 - precision) return -1;
    else if (dx1 > dx2 + precision) return 1;
    else return CompareForwardAngles(pp1, pp2);
  } else
  {
    SetDx(pp1);
    if (pp1->dx == horizontal) {
      if (ofForwardBound & pp1->flags) pp2 = pp1->next; else pp2 = pp1->prev;
      if (pp2->pt.X > pp1->pt.X) return -1; else return 1;
    }
    double pp1X = pp1->pt.X + (pp2->pt.Y - pp1->pt.Y) * pp1->dx;
    if (pp1X < pp2->pt.X - precision) return 1;
    else if (pp1X > pp2->pt.X + precision) return -1;
    else
    {
      if (ofForwardBound & pp2->flags)
        pp2Next = pp2->next; else
        pp2Next = pp2->prev;
      pp1Next = pp2Next;
      do {
        double r = GetR(pp1->pt, pp2->pt, pp2Next->pt);
        if (std::fabs(r) < precision) {
          if (ofForwardBound & pp2->flags)
            pp2Next = pp2->next; else
            pp2Next = pp2->prev;
          continue;
        }
        else if (r > 0) return -1;
        else return 1;
      }
      while (pp2Next != pp1Next); //ie avoids a very rare endless loop
      return 1;
    }
  }
}
//------------------------------------------------------------------------------

bool NextIsBottom(TPolyPt* p)
{
  TPolyPt* pp = p->next;
  while (pp->next->pt.Y == pp->pt.Y) pp = pp->next;
  if (pp->next->pt.Y > pp->pt.Y) return false;
  else return true;
}
//------------------------------------------------------------------------------

void UpdateBounds(SkipList<TPolyPt*>& sl, SkipNode* sn, const double Y)
{
  TPolyPt* pp = (TPolyPt*)sn->item;
  for (;;)
  {
    TPolyPt* pp2;
    if (ofForwardBound & pp->flags)
      pp2 = pp->next; else pp2 = pp->prev;
    if (pp2->pt.Y < Y - tolerance) break;
    pp = pp2;
    if (ofTop & pp->flags) break;
  }

  //nb: DeleteItem() isn't safe here because of wCompare function
  if (ofTop & pp->flags)
    sl.Delete(sn); else
    sn->item = pp;
}
//------------------------------------------------------------------------------

void Clipper::FixOrientation()
{
  //Preconditions:
  //1. All output polygons are simple polygons (ie no self-intersecting edges)
  //2. While output polygons may touch, none of them overlap other polygons.
  SkipList<TPolyPt*> queue(blCompare);

  for (PolyPtList::size_type i = 0; i < m_PolyPts.size(); ++i)
    if (m_PolyPts[i])
    {
      //first, find the lowest left most PPolyPt for each polygon ...
      TPolyPt* p = (TPolyPt*)m_PolyPts[i];

      TPolyPt* lowestP = p;
      p = p->next;
      do
      {
        if (p->pt.Y > lowestP->pt.Y) lowestP = p;
        else if (p->pt.Y == lowestP->pt.Y &&
          p->pt.X <= lowestP->pt.X) lowestP = p;
        p = p->next;
      }
      while ( p != (TPolyPt*)m_PolyPts[i] );

      //dispose of any invalid polygons here ...
      p = lowestP;
      if (p->next == p || p->prev == p->next)
      {
        DisposePolyPts(p);
        m_PolyPts[i] = 0;
        continue;
      }
      m_PolyPts[i] = lowestP;
      TOrientationFlag tmpFlag;
      if (IsClockwise(lowestP)) tmpFlag = ofCW; else tmpFlag = ofEmpty;
      lowestP->flags = tmpFlag;
      bool lowestPending = true;

      //loop around the polygon, build 'bounds' for each polygon
      //and add them to the queue ...
      do
      {
        while (p->next->pt.Y == p->pt.Y) p = p->next; //ignore horizontals

        p->flags = tmpFlag | ofForwardBound;
        if (lowestPending && (ofCW & lowestP->flags))
        {
          p->flags = p->flags | ofBottomMinima;
          lowestPending = false;
        }

        queue.InsertItem(p);
        //go up the bound ...
        while (p->next->pt.Y <= p->pt.Y)
        {
          p->next->flags = tmpFlag | ofForwardBound;
          p = p->next;
        }
        p->flags = p->flags | ofTop;
        //now add the reverse bound (also reversing the bound direction) ...
        while (!NextIsBottom(p))
        {
          p->next->flags = tmpFlag;
          p = p->next;
        }

        if (p->next->pt.Y == p->next->next->pt.Y)
        {
          p = p->next;
          p->flags = tmpFlag;
          if ( !(ofCW & lowestP->flags) && PointsEqual(p->pt, lowestP->pt) &&
            lowestPending )
          {
            p->flags = p->flags | ofBottomMinima;
            lowestPending = false;
          }
          queue.InsertItem(p);
          while (p != lowestP && p->next->pt.Y == p->pt.Y) p = p->next;
        } else
        {
          p = DuplicatePolyPt(p);
          p->pt = p->next->pt;
          p->flags = tmpFlag;
          if ( !(ofCW & lowestP->flags) && PointsEqual(p->pt, lowestP->pt) )
            p->flags = p->flags | ofBottomMinima;
          queue.InsertItem(p);
          p = p->next;
          while (p != lowestP && p->next->pt.Y == p->prev->pt.Y) p = p->next;
        }
      }
      while (p != lowestP);
    }

    if (!queue.Count()) return;
    SkipList<TPolyPt*> workList(wlCompare);

    TPolyPt* p = queue.PopFirst();
    workList.InsertItem(p);
    p->flags = p->flags | ofClockwise;

    for (;;)
    {
      p = queue.PopFirst();
      if (!p) break;

      SkipNode* sn = workList.First();
      while (sn)
      {
        //get the next item in workList in case sn is about to be removed ...
        SkipNode* sn2 = sn->next[0];
        //update each bound, keeping them level with the new bound 'p'
        //and removing bounds that are no longer in scope ...
        //nb: Bounds never intersect other bounds so UpdateBounds() should
        //not upset the order of the bounds in worklist.
        UpdateBounds(workList, sn, p->pt.Y);
        sn = sn2;
      }

      //insert the new bound into WorkList ...
      sn = workList.InsertItem(p);

      //if this is the bottom bound of a polyon,
      //then calculate the polygon's true orientation ...
      if (ofBottomMinima & p->flags)
      {
        SkipNode* sn2 = workList.First();
        bool isCW = true;
        while (sn2 != sn)
        {
          isCW = !isCW;
          sn2 = sn2->next[0];
        }
        if (isCW) p->flags = p->flags | ofClockwise;
      }
    }

  for (PolyPtList::size_type i = 0; i < m_PolyPts.size(); ++i)
    if (m_PolyPts[i])
    {
      p = (TPolyPt*)m_PolyPts[i];
      do
      {
        if (ofBottomMinima & p->flags) break;
        p = p->next;
      }
      while (p != (TPolyPt*)m_PolyPts[i]);
      if (!(ofBottomMinima & p->flags))
        throw clipperException("FixOrientation error");
      if ( (p->flags & (ofCW |ofClockwise)) == ofCW  ||
         (p->flags & (ofCW |ofClockwise)) == ofClockwise )
          ReversePolyPtLinks(*p);
      p = FixupOutPolygon2(p);
      m_PolyPts[i] = p;
    }

}
//------------------------------------------------------------------------------

} //namespace clipper


