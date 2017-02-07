#include <iostream>
#include <regex>
#include <fstream>
#include <list>
#include "heapClass.h"
#include <gtk/gtk.h>
#include <string>
#include "timer.h"
#include <math.h>
#define M_PI 3.1415926
#define EARTH_RADIUS  6378.137
#define RANGE          5000
#define pi 3.14159265358979323846
#define earthRadiusKm 6371000
using namespace std;
template <typename CharT, typename Traits,
          typename Allocator = std::allocator<CharT>>
auto read_stream_into_string(
    std::basic_istream<CharT, Traits>& in,
    Allocator alloc = {})
{
    std::basic_ostringstream<CharT, Traits, Allocator>
    ss(std::basic_string<CharT, Traits, Allocator>(
           std::move(alloc)));

    if (!(ss << in.rdbuf()))
        throw std::ios_base::failure{"error"};

    return ss.str();
}
#define PRIME_HASH_SIZE 111

#define BUFSIZ   20000
vector<list<node>> vecQ0;
bool getNodesInEachWay(string& strRead,list<uint64_t>& lstIds);
bool checkListHasInt(int64_t id,list<uint64_t>& lst);
void improveListOfId(list<uint64_t>&lst);
bool pushIntoQ0(node& nd);
//bool fetchNodeFromQ0(uint64_t id,node& nd);
node& fetchNodeFromQ0(uint64_t id,bool& bFlagReturn);

void addEdgeFromList(list<uint64_t>& lst);
double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d);
bool updateListEnds(node nd,list<node>&q1,list<node>&lstEnds);
bool updateDistanceAndPreviousNodes2(list<node>lstChange,double distDecreaas,list<node>&q1,list<node>&lstEnds);
bool getEndNodes(int idStart,list<node>& q1,list<node>&lstEnds);
bool getNode(uint64_t id,node& nd);
int getListSize(list<node>& lst);//for gdb
bool searchInQ0(int index,list<node>&q0);
void strengthListIds(list<uint64_t>&lst);


template <typename CharT, typename Traits,
          typename CharO = CharT,
          typename Allocator = std::allocator<CharO>>
auto read_file_into_deque(
    std::basic_istream<CharT, Traits>& in,
    Allocator alloc = {})
{
  static_assert(
    std::is_same<CharT, CharO>::value ||
      std::is_same<CharT,
        CharO>::value ||
      std::is_same<CharT,
        CharO>::value,
    "char type of deque must be same "
    "as stream char type "
    "(possibly signed or unsigned)");

  using std::begin;
  using std::end;

  auto const chunk_size = std::size_t{BUFSIZ};

  auto container = std::deque<CharO, Allocator>(
    std::move(alloc));

  auto chunk = std::array<CharO, chunk_size>{};

  while (
      in.read(
        reinterpret_cast<CharT*>(chunk.data()),
        chunk.size()) ||
      in.gcount())
    container.insert(end(container),
                     begin(chunk),
                     begin(chunk) + in.gcount());

  return container;
}
string::iterator getLeftOverFromBlock(string& strBlock,int nSize);

struct wayNodes
{
    uint64_t id;
    list<uint64_t>lstNodes;//nodes connects to id node
};
struct latLonScope
{
    double minLat;
    double minLon;
    double maxLat;
    double maxLon;
    double latDiff;
    double lonDiff;
    bool bFlag;
    latLonScope(){bFlag=false;};
};
#define SIZEHASH        503
#define SIZEHASHEDGES   4993
void addEdge(uint64_t id1,uint64_t id2,vector<list<wayNodes>>& ways,vector<list<node>>& nodes);
void drawMap(latLonScope& scope);
void updateScope(list<node>lstEnds,uint64_t idStartOrigin,latLonScope& scope);
//bool beginDraw(latLonScope& scope);
bool beginDrawBar(latLonScope& scope);
struct pointPixel
{
    double x;
    double y;
    //pointPixel(){x=0;y=0;}
};
struct coordinate
{
   double lat;
   double lon;
};
struct gPointerPass
{
   vector<list<node>> hNodes;
   vector<list<wayNodes>> hWays;
   latLonScope scopeArea;
   gPointerPass(){node temp;list<node> lstTemp;lstTemp.push_back(temp);hNodes.push_back(lstTemp);
                    wayNodes wayTemp;wayTemp.id=0;list<uint64_t>lst={0};wayTemp.lstNodes=lst;list<wayNodes>lstBar;lstBar.push_back(wayTemp);hWays.push_back(lstBar);}
};

struct line
{
    pointPixel pt1;
    pointPixel pt2;
};
pointPixel transformCoordinate(vector<list<node>>& hNodes,uint64_t id1,latLonScope& scope);

vector<list<node>> hashNodes;
vector<list<wayNodes>> hashWays;
latLonScope scope;

 timer t;
 void funcLogTime(timer& t)
{
    cout<<"drawing lines take "<<t.elapsed()<<endl;
}
static gboolean on_draw(GtkWidget *widget,gpointer pData)
{
    //gPointerPass* pVal=(gPointerPass*)pData;
    vector<line> vecLog;
    line ptToLog;

    //t.elapsed();
    vector<list<node>>& hNodes=hashNodes;
    vector<list<wayNodes>>& hWays=hashWays;
    latLonScope* pScope=(latLonScope*)pData;
    latLonScope areaScope=scope;

    ////////////////////////////
    cairo_t *cr;//声明一个指向cairo_t结构的指针S
     cr = gdk_cairo_create(gtk_widget_get_window(widget));//产生一个对象（指针）
    cairo_set_source_rgb(cr,1,1,1);

    cairo_fill(cr);
    cairo_set_line_width(cr,1);
    pointPixel pt1;
    pointPixel pt2;
    //for(auto it=hWays.begin();it!=hWays.end();it++)
    int nLoopHwaysCountLog=0;
  for(auto it=hWays.begin();nLoopHwaysCountLog<SIZEHASHEDGES;it++,nLoopHwaysCountLog++)
    {
        list<wayNodes>& lstTemp=*it;
        for(auto itSub=lstTemp.begin();itSub!=lstTemp.end();itSub++)
        {
            wayNodes& var=*itSub;
            uint64_t& idHead=var.id;
            pt1=transformCoordinate(hNodes,idHead,areaScope);
            double radius1=0;
            double raduis2=360.0*(M_PI/180.0);

            //cairo_arc(cr,pt1.x,pt1.y,2,radius1,raduis2);
            //cairo_set_source_rgba (cr, 0, 1.0, 0, 1.0);
            //cairo_stroke(cr);


            cairo_move_to(cr,pt1.x,pt1.y);
            string strMark=to_string(idHead);
             cairo_set_source_rgb(cr, 0,0,0);
            list<uint64_t>& lstDeepSub=var.lstNodes;
            int nSize=lstDeepSub.size();
            int iLog=0;
            for(auto itDeepSub=lstDeepSub.begin();itDeepSub!=lstDeepSub.end();)
            {
               uint64_t& id2=*itDeepSub;
               pt2=transformCoordinate(hNodes,id2,areaScope);
               cairo_line_to(cr,pt2.x,pt2.y);
               advance(itDeepSub,1);
               ptToLog.pt1=pt1;
               ptToLog.pt2=pt2;
               vecLog.push_back(ptToLog);
               cairo_stroke(cr);
            }
        }
    }


        ///////////////////////////

    funcLogTime(t);
    cairo_stroke(cr);
    cairo_destroy(cr);

}

int widthOfScreen=2000;
int heightOfScreen=1000;
coordinate getCoordinate(vector<list<node>>& hNodes,uint64_t& id1)
{
   coordinate coor;
   list<node>& lstTemp=hNodes[id1%SIZEHASH];
   auto it=lstTemp.begin();
   for(;it!=lstTemp.end();it++)
   {
       node& var=*it;
       if(var.id==id1)
         break;
   }

   if(it==lstTemp.end())
      {
         cout<<"Error,Not find the id in corresponding hashNodes slot!"<<endl;
         return coor;
      }
      coor.lat=it->lat;
      coor.lon=it->lon;
      return coor;

}


pointPixel transformCoordinate(vector<list<node>>& hNodes,uint64_t id1,latLonScope& scope)
{
     pointPixel pt;
     coordinate coor1=getCoordinate(hNodes,id1);
     double dDiffLon=coor1.lon-scope.minLon;
     pt.x=(dDiffLon)/(scope.lonDiff)*widthOfScreen;
     double dDiffLat=scope.maxLat-coor1.lat;
     pt.y=(dDiffLat/(scope.latDiff))*heightOfScreen;
     return pt;
}


int main()
{

    srand(time(0));
    t.elapsed();

     GtkWidget *window;
    gtk_init(0,NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);



    hashNodes.resize(SIZEHASH);
    hashWays.resize(SIZEHASHEDGES);

    vecQ0.resize(PRIME_HASH_SIZE);
    ifstream in("shanghai.osm");

    if(!in.good())
    {
        cout<<"fail to open file"<<endl;
        return 0;
    }
    auto deqRead=read_file_into_deque(in);

    const std::sregex_token_iterator itSearch;
    const std::regex expressionSearch("<node[ ]+id=\"(\\d+)\"[ ]+lat=\"([\\d]+.[\\d]+)\"[ ]+lon=\"([\\d]+.[\\d]+)\"");
    const std::sregex_token_iterator finistNull;
    in.close();
    std::vector<int>vec= {1,2,3};
    node nTemp;
    cout<<"Done"<<endl;
    //std::vector<node>vecNodes;
    string strRead;
    string strBlockLeftOver{};  

    int nLeftOverInBlock=0;
    int iLoopLog=0;
    bool bNode=true;
    bool bWay=false;

    const std::regex expressionWay("<way[ ]+id=\"(\\d+)\".+.*[\\s\\S]+?<\\/way>");
    const std::regex extractWay("<nd[ ]+ref=\"(\\d+)\"[\\s\\S]+?>");
    list<uint64_t>lstWayNodes;
    string strWayHead="<way";
    string strRelationHead="<relation";
    int nDifferFromBegin;    //string::iterator itWayHead;
    uint64_t uCountLog=0;
    int nAdvance;
    uint64_t uTotalCount=deqRead.size();
    //////////////



    /////////////////////////
    for(auto itDeque=deqRead.begin(); itDeque!=deqRead.end();iLoopLog++)
    {





        strRead.clear();
        auto itTemp=itDeque;
        //int nWatch=deqRead.size();
        int nToEnd=deqRead.end()-itDeque;
        nAdvance=min(nToEnd,BUFSIZ);
        uCountLog+=nAdvance;
        int nSizeStrRead=nLeftOverInBlock+nAdvance;
        strRead.resize(nSizeStrRead);
        for(auto it=strRead.begin(),itBar=strBlockLeftOver.begin();it!=strRead.end()&&itBar!=strBlockLeftOver.end();it++,itBar++)
        {
            *it=*itBar;
        }
       
        advance(itDeque,nAdvance);
     
        for(auto it=strRead.begin()+nLeftOverInBlock;it!=strRead.end()&&itTemp!=itDeque;it++,itTemp++)
        {
            *it=*itTemp;
        }




        if(bNode==true)
        {
            auto itWayheadBar=search(strRead.begin(),strRead.end(),strWayHead.begin(),strWayHead.end());
            if(itWayheadBar!=strRead.end())
            {
                bWay=true;
                nDifferFromBegin=itWayheadBar-strRead.begin();


            }
            //strHalfNodeHalfWay.assign(itWayheadBar,strRead.end());
            //itWayHead=itWayheadBar;
        }
        else if(bWay==true)
        {
            auto itRelation=search(strRead.begin(),strRead.end(),strRelationHead.begin(),strRelationHead.end());
            auto itWay=search(strRead.begin(),strRead.end(),strWayHead.begin(),strWayHead.end());
            if(itWay==strRead.end()&&itRelation!=strRead.end())
                break;
        }


        //////////////////
        if(bNode==true)
        {


            for(std::sregex_token_iterator it(strRead.begin(),strRead.end(),expressionSearch,vec); it!=finistNull;)
            {
                //nTemp.id=atoi((*it).str().c_str());
                stringstream sstream((*it).str());
                sstream>>nTemp.id;
                if(nTemp.id==140737353975056)
                {
                    cout<<"node shit"<<endl;
                }
                it++;
                nTemp.lat=atof((*it).str().c_str());
                it++;
                nTemp.lon=atof((*it).str().c_str());
                it++;
                //vecNodes.push_back(nTemp);
                list<node>& lstWatch=hashNodes[nTemp.id%SIZEHASH];
                auto itSearchRepeat=find(lstWatch.begin(),lstWatch.end(),nTemp);
                if(itSearchRepeat!=lstWatch.end())
                   continue;
                lstWatch.push_back(nTemp);
                if(scope.bFlag==false)
                {
                    scope.minLat=nTemp.lat;
                    scope.maxLat=nTemp.lat;
                    scope.minLon=nTemp.lon;
                    scope.maxLon=nTemp.lon;
                    scope.bFlag=true;
                }
                else
                {
                   if(nTemp.lat<scope.minLat)
                      scope.minLat=nTemp.lat;
                    else if(nTemp.lat>scope.maxLat)
                            scope.maxLat=nTemp.lat;
                    if(nTemp.lon<scope.minLon)
                       scope.minLon=nTemp.lon;
                    else if(nTemp.lon>scope.maxLon)
                             scope.maxLon=nTemp.lon;
                }
                //hashNodes[nTemp.id%SIZEHASH].push_back(nTemp);
            }
            if(bWay==true)
            {
                bNode=false;

                //pass string to read way
                for(std::sregex_token_iterator it(strRead.begin()+nDifferFromBegin,strRead.end(),expressionWay,0); it!=finistNull; it++)
                {
                    //cout<<(*it).str().c_str()<<endl;
                    string strBar=(*it).str();
                    list<uint64_t> lstTemp;
                    for(std::sregex_token_iterator itExtract(strBar.begin(),strBar.end(),extractWay,1); itExtract!=finistNull; itExtract++)
                    {
                        auto sWatch=(*itExtract).str();
                        stringstream sstream((*itExtract).str());
                        uint64_t idExtract;
                        sstream>>idExtract;
                        //uint64_t var=atoi((*itExtract).str().c_str());
                        lstTemp.push_back(idExtract);
                    }
                    //improveListOfId(lstTemp);
                    //strengthListIds(lstTemp);
                    addEdgeFromList(lstTemp);
//
                }
            }
        }
        else
           {


            //pass string to read way
            deque<char> strTemp;
            strTemp.resize(strRead.size());
            auto itBar=strRead.begin();
            for(auto it=strTemp.begin();it!=strTemp.end();)
            {
                *it++=*itBar++;
            }

            string varIndex1="<way";

            string varIndex2="</way";
            auto itIndex1=search(strTemp.begin(),strTemp.end(),varIndex1.begin(),varIndex1.end());
            auto itIndex1Trailing=find(itIndex1,strTemp.end(),'>');
            auto itIndex2=search(itIndex1Trailing,strTemp.end(),varIndex2.begin(),varIndex2.end());
            while(itIndex1!=strTemp.end()&&itIndex1Trailing!=strTemp.end()&&itIndex2!=strTemp.end())
            {
                string strBar;
                list<uint64_t>lstTemp;
                int nTempCount=itIndex2-itIndex1Trailing-1;
                strBar.resize(nTempCount);
                strBar.assign(itIndex1Trailing+1,itIndex2);
                 for(std::sregex_token_iterator itExtract(strBar.begin(),strBar.end(),extractWay,1); itExtract!=finistNull; itExtract++)
                {
                    auto sWatch=(*itExtract).str();
                    stringstream sstream(sWatch);

                    uint64_t var;//=//atoi((*itExtract).str().c_str());
                    sstream>>var;
                    lstTemp.push_back(var);


                }


                //improveListOfId(lstTemp);
                //strengthListIds(lstTemp);
                addEdgeFromList(lstTemp);

                strTemp.erase(itIndex1Trailing+1,itIndex2);
                itIndex1=search(itIndex2,strTemp.end(),varIndex1.begin(),varIndex1.end());
                itIndex1Trailing=find(itIndex1,strTemp.end(),'>');
                itIndex2=search(itIndex1Trailing,strTemp.end(),varIndex2.begin(),varIndex2.end());

            }


        }
        auto itLeftOver=getLeftOverFromBlock(strRead,nSizeStrRead);
        strBlockLeftOver.assign(itLeftOver,strRead.end());
        nLeftOverInBlock=strRead.end()-itLeftOver;

    }





     cout<<"put into container takes "<<t.elapsed()<<endl;

    scope.latDiff=scope.maxLat-scope.minLat;
    scope.lonDiff=scope.maxLon-scope.minLon;
    gPointerPass valPass;
    valPass.hNodes=hashNodes;
    valPass.hWays=hashWays;
    valPass.scopeArea=scope;

    //GtkWidget* button;
   // gtk_container_add (GTK_CONTAINER (window), button);

    //GtkWidget* box1 = gtk_hbox_new(FALSE, 0);

    /* put the box into the main window. */
    //gtk_container_add (GTK_CONTAINER (window), box1);

    /* creates a new button with the label "Button 1". */
    //button = gtk_button_new_with_label ("Button 1");
        //gtk_box_pack_start(GTK_BOX(box1), button, TRUE, TRUE, 0);
    beginDrawBar(scope);
//    drawMap(scope);
//
//    g_signal_connect(G_OBJECT(window),"draw",G_CALLBACK(on_draw),(gpointer)&scope);
//    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
//
//    gtk_window_set_title(GTK_WINDOW(window), "cairo，你好");
//    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
//    gtk_window_set_default_size(GTK_WINDOW(window),widthOfScreen,heightOfScreen);
//
//    gtk_widget_set_app_paintable(window,TRUE);
//    //gtk_widget_show(button);
//
//    //gtk_widget_show(box1);
//    gtk_widget_show_all(window);
    //gtk_widget_show(button);


    gtk_main();


    return 0;
}

void addEdge(uint64_t id1,uint64_t id2,vector<list<wayNodes>>& ways,vector<list<node>>& nodes)
{
    if(id1==1439717868||id2==1439717868)
    {
        int debug=0;
    }
    list<node>& lstNodeUpdate=nodes[id1%SIZEHASH];
    list<node>::iterator itNode=lstNodeUpdate.begin();
    bool bFlagNodeExist=true;
    bool bFlagNodeExist2=true;
    for(;itNode!=lstNodeUpdate.end();itNode++)
    {
       if((*itNode).id==id1)
         {
             itNode->bHasWay=true;

             break;

         }
    }
    if(itNode==lstNodeUpdate.end())
    {
        std::cout<<"Error,there is no corresponding node stored!"<<std::endl;
        return;
        //bFlagNodeExist=false;
    }
    list<node>& lstNodeUpdate2=nodes[id2%SIZEHASH];
    for(itNode=lstNodeUpdate2.begin();itNode!=lstNodeUpdate2.end();itNode++)
    {
       if((*itNode).id==id2)
         {
             itNode->bHasWay=true;

             break;

         }
    }
    if(itNode==lstNodeUpdate2.end())
    {
        std::cout<<"Error,there is no corresponding node stored!"<<std::endl;
        return;
        //bFlagNodeExist2=false;

    }


    list<wayNodes>& lstTemp=ways[id1%SIZEHASHEDGES];
    auto it=lstTemp.begin();
    for(; it!=lstTemp.end(); it++)
    {
        wayNodes var=*it;
        if(id1==var.id)
            break;
    }
    if(it==lstTemp.end())
    {
        wayNodes temp;
        temp.id=id1;
        temp.lstNodes.push_back(id2);
        lstTemp.push_back(temp);

    }
    else
    {
        wayNodes& var2=*it;
        auto it2=var2.lstNodes.begin();
        for(; it2!=var2.lstNodes.end(); it2++)
        {
            if((*it2)==id2)
                break;
        }
        if(it2==var2.lstNodes.end())
        {
            var2.lstNodes.push_back(id2);

        }
    }
    ///////////////////////////////////////////////
    list<wayNodes>& lstTemp2=ways[id2%SIZEHASHEDGES];
    auto it2=lstTemp2.begin();
    for(; it2!=lstTemp2.end(); it2++)
    {
        wayNodes var=*it2;
        if(id2==var.id)
            break;
    }
    if(it2==lstTemp2.end())
    {
        wayNodes temp;
        temp.id=id2;
        temp.lstNodes.push_back(id1);
        lstTemp2.push_back(temp);

    }
    else
    {
        wayNodes& var2=*it2;
        auto it3=var2.lstNodes.begin();
        for(; it3!=var2.lstNodes.end(); it3++)
        {
            if((*it3)==id1)
                break;
        }
        if(it3==var2.lstNodes.end())
        {
            var2.lstNodes.push_back(id1);

        }
    }

}

string::iterator getLeftOverFromBlock(string& strBlock,int nSizeBlock)
{
    auto itTraverse=strBlock.end()-1;
    //auto watch=*itReverse;
    int iLoad=0;
    bool bFlagNodeWayExist=false;
    for(int iCountLoops=0; itTraverse!=strBlock.begin(); itTraverse--,iCountLoops++)
    {
        auto watch=*itTraverse;

        if(iCountLoops<3)
        {
            if(*itTraverse=='<')
                iLoad-=1;
            else if(*itTraverse=='>')
                iLoad+=1;
            continue;
        }
        if(iCountLoops==3 )
       {

         if(*itTraverse=='<')
            {

                iLoad-=1;
                string strTemp;
                //int nFromRbegin=itReverse-strBlock.rbegin();
                //auto itFromBegin=strBlock.begin()+(nSizeBlock-1-nFromRbegin);
                strTemp.assign(itTraverse,strBlock.end());
                //strTemp.assign(strBlock.rbegin(),itReverse+1);
                if(strTemp=="<way")
                {
                    bFlagNodeWayExist=true;
                    break;
                    //return itReverse;

                }
            }
            else if(*itTraverse=='>')
                iLoad+=1;

            continue;

        }

        if(*itTraverse=='<')
        {

            iLoad-=1;

            string strTemp;
            //int nFromRbegin=itReverse-strBlock.rbegin();
            //auto itFromBegin=strBlock.begin()+(nSizeBlock-1-nFromRbegin);
            strTemp.assign(itTraverse,itTraverse+4);
            if(strTemp=="<way")
            {
                bFlagNodeWayExist=true;
                break;
            }
            else
            {

               strTemp.assign(itTraverse,itTraverse+5) ;
                if(strTemp=="<node")
                {
                    bFlagNodeWayExist=true;
                    break;
                }
            }


        }
        else if(*itTraverse=='>')
        {
            iLoad+=1;
        }
    }
    if(itTraverse==strBlock.begin()&&bFlagNodeWayExist==false)
    {
        return itTraverse;
    }
    if(iLoad!=0)

        {
            //int nFromRbegin=itReverse-strBlock.rbegin();
            //auto itFromBegin=strBlock.begin()+(nSizeBlock-1-nFromRbegin);


            return itTraverse;//itReverse;
        }
    else
        return strBlock.end();


}
double rad(double d)
{
   return M_PI / 180.0;
}


bool getNode(uint64_t id,node& nd)
{
    bool bFlag=false;
    list<node>& lstNode=hashNodes[id%SIZEHASH];
    for(auto it=lstNode.begin();it!=lstNode.end();it++)
    {
        node& var=*it;
        if(var.id==id)
        {
            bFlag=true;
            nd=var;
            break;

        }
    }
    return bFlag;
}
bool getListWayNodes(uint64_t id,list<uint64_t>& lst)
{
    bool bFlag=false;
    auto lstWay=hashWays[id%SIZEHASHEDGES];
    for(auto it=lstWay.begin();it!=lstWay.end();it++)
    {
        wayNodes var=*it;
        if(var.id==id)
        {
            bFlag=true;
            lst.assign(var.lstNodes.begin(),var.lstNodes.end());
        }
    }
    return bFlag;
}
bool updateDistanceAndPreviousNodes(node nd,double distDecreaase,list<node>&q1,list<node>&lstEnds)
{
    list<uint64_t>lstConn;
    if(!getListWayNodes(nd.id,lstConn))
        return false;
    bool bFlag1=false;
    bool bFlag2=false;
    list<node> lstNodeChange;
    for(auto it=lstConn.begin();it!=lstConn.end();it++)
    {

        bool bFetch;
        node& nodeFetchedFromeQ0=fetchNodeFromQ0(nd.id,bFetch);


        if(bFetch)
        {
            nodeFetchedFromeQ0.distance-=distDecreaase;
            lstNodeChange.push_back(nodeFetchedFromeQ0);
        }
        for(auto it3=lstEnds.begin();it3!=lstEnds.end();)
        {
            node var=*it3;
            auto it4=it3;
            it3++;
            if(var.id==(*it)&&var.idPrev==nd.id)
            {
                var.distance-=distDecreaase;
                if(var.distance<RANGE)
                {
                    //q1.push_back(var);
                    if(!updateListEnds(var,q1,lstEnds))
                        return false;
                    lstEnds.erase(it4);
                }
                break;
            }
        }
    }
    return updateDistanceAndPreviousNodes2(lstNodeChange,distDecreaase,q1,lstEnds);


}

bool updateListEnds(node nd,list<node>&q1,list<node>&lstEnds)
{
    list<uint64_t>lstConn;
    if(!getListWayNodes(nd.id,lstConn))
        return false;
    bool bFlag3=false;
    double distInQ0;
    //double dist1;
        for(auto itConn=lstConn.begin();itConn!=lstConn.end();itConn++)
        {
            uint64_t idConn=*itConn;
            ///////////check to see if q0 contains it
//            auto itq0=q0.begin();
//            for(;itq0!=q0.end();itq0++)
//            {
//                node& nodeQ0=*itq0;
//                if(nodeQ0.id==idConn)
//                {
//                    bFlag3=true;
//                    distInQ0=nodeQ0.distance;
//                    break;
//                }
//            }
            bool bFetch;
            node& nodeFetchQ0=fetchNodeFromQ0(idConn,bFetch);
            //bFlag3=fetchNodeFromQ0(idConn,nodeFetchQ0);

            if(bFetch==true)
            {
                distInQ0=nodeFetchQ0.distance;
                node& nodeConn=nodeFetchQ0;

                double dist1=nd.distance+distanceEarth(nd.lat,nd.lon,nodeConn.lat,nodeConn.lon);
                double distDecrease=distInQ0-dist1;
                if(dist1<distInQ0)
                {
                    /////////////update distance and previous node id

                    nodeConn.distance=dist1;
                    nodeConn.idPrev=nd.id;//to be continued
                    updateDistanceAndPreviousNodes(nodeConn,distDecrease,q1,lstEnds);
                }

            }
            else
            {
                //to be continued
                node nodeConn;
                if(!getNode(idConn,nodeConn))
                    {
                        nodeConn.id=idConn;
                        nodeConn.idPrev=nd.id;
                        nodeConn.distance=nd.distance;
                        q1.push_back(nodeConn);
                        continue;
                        }//return false;
                nodeConn.idPrev=nd.id;
                nodeConn.distance=nd.distance+distanceEarth(nd.lat,nd.lon,nodeConn.lat,nodeConn.lon);
                if(nodeConn.distance>=RANGE)
                {
                    lstEnds.push_back(nodeConn);
                    pushIntoQ0(nodeConn);
                    continue;
                }
                q1.push_back(nodeConn);

            }
        }
        return true;
}

bool updateDistanceAndPreviousNodes2(list<node>lstChange,double distDecreaase,list<node>&q1,list<node>&lstEnds)
{
    for(auto itOut=lstChange.begin();itOut!=lstChange.end();itOut++)
    {
        node nd=*itOut;

    list<uint64_t>lstConn;
    if(!getListWayNodes(nd.id,lstConn))
        return false;
    for(auto it=lstConn.begin();it!=lstConn.end();it++)
    {

        bool bFetch;
        node& nodeFetch=fetchNodeFromQ0(nd.id,bFetch);
        if(bFetch)
        {
            nodeFetch.distance-=distDecreaase;
        }
        for(auto it3=lstEnds.begin();it3!=lstEnds.end();)
        {
            node var=*it3;
            auto it4=it3;
            it3++;
            if(var.id==(*it)&&var.idPrev==nd.id)
            {
                var.distance-=distDecreaase;
                if(var.distance<RANGE)
                {
                    //q1.push_back(var);
                    if(!updateListEnds(var,q1,lstEnds))
                        return false;
                    lstEnds.erase(it4);
                }
                break;
            }
        }
    }

    }
    return true;
}


bool getEndNodes(int idStart,list<node>& q1,list<node>&lstEnds)
{
    list<wayNodes>& lstWayNeighbour=hashWays[idStart%SIZEHASHEDGES];
    node node0;
    if(!getNode(idStart,node0))
        return false;
    bool bFlag1=false;
    list<uint64_t> lstNodes1;
    list<wayNodes>::iterator it1=lstWayNeighbour.begin();
    for(;it1!=lstWayNeighbour.end();it1++)
    {
        wayNodes& way=*it1;
        if(way.id==idStart)
        {
            lstNodes1.assign(way.lstNodes.begin(),way.lstNodes.end());
            break;
        }

    }
    if(it1==lstWayNeighbour.end())
        return false;

    for(auto it11=lstNodes1.begin();it11!=lstNodes1.end();it11++)
    {
        uint64_t idTemp=*it11;
        node nodeTemp;
        if(!getNode(idTemp,nodeTemp))
           {
               nodeTemp.idPrev=node0.id;
               nodeTemp.distance=0;
               q1.push_back(nodeTemp);
               continue;

           }

        nodeTemp.idPrev=node0.id;
        double dist=distanceEarth(nodeTemp.lat,nodeTemp.lon,node0.lat,node0.lon);
        if(dist<RANGE)
        {
            nodeTemp.distance=dist;
            q1.push_back(nodeTemp);
            bFlag1=true;
        }
    }
    //q0.push_back(node0);

    if(bFlag1==false)
        return false;
    pushIntoQ0(node0);
    bool bFlag2=false;
    while(!q1.empty())
    {
        bool bFlag2=false;
        node var=*(q1.begin());
        //q0.push_back(var);
        pushIntoQ0(var);
        q1.erase(q1.begin());
        list<uint64_t> lstConn;

        bFlag2=getListWayNodes(var.id,lstConn);
        if(!bFlag2)
            return false;
        bool bFlag3=false;
        double distInQ0;
        for(auto itConn=lstConn.begin();itConn!=lstConn.end();itConn++)
        {
            uint64_t idConn=*itConn;
            bFlag3=false;
            ///////////check to see if q0 contains it


            node& nodeFetched=fetchNodeFromQ0(idConn,bFlag3);
            //bFlag3=fetchNodeFromQ0(idConn,nodeFetched);
            if(bFlag3==true)
            {
                node& nodeConn=nodeFetched;

                double dist1=var.distance+distanceEarth(var.lat,var.lon,nodeConn.lat,nodeConn.lon);
                double distDecrease=distInQ0-dist1;
                if(dist1<distInQ0)
                {
                    /////////////update distance and previous node id

                    nodeConn.distance=dist1;
                    nodeConn.idPrev=var.id;//to be continued
                    updateDistanceAndPreviousNodes(nodeConn,distDecrease,q1,lstEnds);
                }
                else
                    {
                    }

            }
            else
            {
                //to be continued
                node nodeConn;
                if(!getNode(idConn,nodeConn))
                   {
                       nodeConn.id=idConn;
                       nodeConn.idPrev=var.id;
                       nodeConn.distance=var.distance;
                       q1.push_back(nodeConn);
                       continue;

                   } //return false;

                nodeConn.idPrev=var.id;
                nodeConn.distance=var.distance+distanceEarth(var.lat,var.lon,nodeConn.lat,nodeConn.lon);
                if(nodeConn.distance>=RANGE)
                {
                    pushIntoQ0(nodeConn);
                    lstEnds.push_back(nodeConn);
                    continue;
                }
                q1.push_back(nodeConn);

            }
        }
    }
    return true;
}

bool searchInQ0(int index,list<node>&q0)
{
    bool bFlag=false;
    for(auto it=q0.begin();it!=q0.end();it++)
    {
        node& temp=*it;
        if(temp.id==index)
        {
            bFlag=true;
            break;
        }

    }
    return bFlag;

}
int getListSize(list<node>& lst)
{
    int nCount=lst.size();
    return nCount;
}

double deg2rad(double deg) {
  return (deg * pi / 180);
}

//  This function converts radians to decimal degrees
double rad2deg(double rad) {
  return (rad * 180 / pi);
}


double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d) {
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = deg2rad(lat1d);
  lon1r = deg2rad(lon1d);
  lat2r = deg2rad(lat2d);
  lon2r = deg2rad(lon2d);
  u = sin((lat2r - lat1r)/2);
  v = sin((lon2r - lon1r)/2);
  return 2.0 * earthRadiusKm * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}

void getLineToStartNode(uint64_t idEndNode,uint64_t idStartDrawNode,list<uint64_t>&lst)
{
    lst.clear();
    lst.push_back(idEndNode);
    bool bFetch=false;
    node ndIndex;
    node nd= fetchNodeFromQ0(idEndNode,bFetch);
    ndIndex=nd;
    while(ndIndex.idPrev!=idStartDrawNode)
    {
         nd=fetchNodeFromQ0(nd.idPrev,bFetch);
         if(bFetch==false)
         {
             cout<<"fetchNodeFromQ0 failed"<<endl;
             return;

         }
         ndIndex=nd;
         lst.push_back(nd.id);

    }
    lst.push_back(idStartDrawNode);

}

bool beginDrawBar(latLonScope& scope)
{
    list<node> q0,q1,lstEnds;
    auto lstNode=hashNodes[0];
    node nodeStart=*(lstNode.begin());
    uint64_t idStartDrawNode=301638556;//shanghai peopele square:301638556;
   bool bFlag=getEndNodes(idStartDrawNode,q1,lstEnds);//songJiang nodeStart.id getEndNodes getEndNodesgetEndNodes
    //updateScope(lstEnds,idStartDrawNode,scope);
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,widthOfScreen,heightOfScreen);
    cr = cairo_create (surface);



    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, widthOfScreen, heightOfScreen);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
       /////////////////////////////////////////
    int nLoopHwaysCountLog=0;
    for(auto it=hashWays.begin();nLoopHwaysCountLog<SIZEHASHEDGES;it++,nLoopHwaysCountLog++)
    {
        list<wayNodes>& lstTemp=*it;
        pointPixel pt1,pt2;
        for(auto itSub=lstTemp.begin();itSub!=lstTemp.end();itSub++)
        {
            wayNodes& var=*itSub;
            uint64_t& idHead=var.id;
            pt1=transformCoordinate(hashNodes,idHead,scope);
            double radius1=0;
            double raduis2=360.0*(M_PI/180.0);

            cairo_set_source_rgb(cr, 0,0,0);
            cairo_set_line_width (cr, 1);
            cairo_move_to(cr,pt1.x,pt1.y);
            string strMark=to_string(idHead);

            list<uint64_t>& lstDeepSub=var.lstNodes;
            int nSize=lstDeepSub.size();
            int iLog=0;
            bool bFlagMoved=true;
            for(auto itDeepSub=lstDeepSub.begin();itDeepSub!=lstDeepSub.end();itDeepSub++)
            {
               uint64_t& id2=*itDeepSub;
               node ndNoUse;
               if(!getNode(id2,ndNoUse))
               {
                   bFlagMoved=false;
                   continue;

               }
               if(bFlagMoved==true)
               {
                   pt2=transformCoordinate(hashNodes,id2,scope);
                cairo_set_source_rgb(cr, 0,0,0);
                cairo_set_line_width (cr, 1);
               cairo_line_to(cr,pt2.x,pt2.y);
               cairo_move_to(cr,pt2.x,pt2.y);
               }
               else
                {
                    pt2=transformCoordinate(hashNodes,id2,scope);
                cairo_set_source_rgb(cr, 0,0,0);
                cairo_set_line_width (cr, 1);
               cairo_move_to(cr,pt2.x,pt2.y);
               bFlagMoved=true;
                }


               cairo_stroke(cr);
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    
    for (auto it=lstEnds.begin();it!=lstEnds.end();it++)
    {
        node nd=*it;
        nd.idPrev=(*it).idPrev;
        pointPixel pt1;//=getCoordinate(hashNodes,nd.id);
       
        list<uint64_t> lstLineNodes;
        getLineToStartNode(nd.id,idStartDrawNode,lstLineNodes);
        //drawFromListIds(cr,lstLineNodes);
          bool bFlagMoved=false;
    pointPixel pt2;
     for(auto itDeepSub=lstLineNodes.begin();itDeepSub!=lstLineNodes.end();itDeepSub++)
    {
       uint64_t& id=*itDeepSub;
       auto itBar=itDeepSub;
       itBar++;
       if(itBar==lstLineNodes.end())
        break;
       uint64_t id2=*itBar;
       node ndNoUse;
       if(!getNode(id,ndNoUse))
       {
           bFlagMoved=false;
           continue;

       }
       cairo_set_source_rgb(cr, 0,0.5,0);
        cairo_set_line_width (cr, 1);

       if(!getNode(id2,ndNoUse))
       {

               cairo_set_source_rgb(cr, 0,0.5,0);
                cairo_set_line_width (cr, 5);
                pt1=transformCoordinate(hashNodes,id,scope);
                cairo_move_to(cr,pt1.x,pt1.y);



       }
       else
       {

               cairo_set_source_rgb(cr, 0,0.5,0);
                cairo_set_line_width (cr, 5);
                pt1=transformCoordinate(hashNodes,id,scope);
                cairo_move_to(cr,pt1.x,pt1.y);
                pt2=transformCoordinate(hashNodes,id2,scope);
                cairo_line_to(cr,pt2.x,pt2.y);


       }
       cairo_stroke(cr);

    }



    }
    double radius1=0;
    double raduis2=360.0*(M_PI/180.0);
    pointPixel origin=transformCoordinate(hashNodes,idStartDrawNode,scope);
    cairo_set_source_rgb(cr, 0.8,0.2,0.2);
    cairo_set_line_width (cr, 10);
    cairo_arc(cr,origin.x,origin.y,5,radius1,raduis2);
    cairo_stroke(cr);

    cairo_surface_write_to_png(surface, "image.png");
    return true;
}
bool beginDraw(latLonScope& scope)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,widthOfScreen,heightOfScreen);
    cr = cairo_create (surface);



    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, widthOfScreen, heightOfScreen);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    ////////////////////////////////////
     list<node> q0,q1,lstEnds;
    auto lstNode=hashNodes[0];
    node nodeStart=*(lstNode.begin());
    uint64_t idStartDrawNode=1439718308;
   bool bFlag=getEndNodes(idStartDrawNode,q1,lstEnds);//songJiang nodeStart.id getEndNodes getEndNodesgetEndNodes
   // bool bFlag=getEndNodes(301638556,q1,lstEnds);//301638556（shanghai人民广场）
    for (auto it=lstEnds.begin();it!=lstEnds.end();it++)
    {
        node nd=*it;
        nd.idPrev=(*it).idPrev;
        pointPixel pt1;//=getCoordinate(hashNodes,nd.id);
         pt1=transformCoordinate(hashNodes,nd.id,scope);
            double radius1=0;
            double raduis2=360.0*(M_PI/180.0);
        cairo_set_source_rgb(cr, 0,0,0);
        cairo_set_line_width (cr, 1);
        cairo_move_to(cr,pt1.x,pt1.y);
        bool bMoveAlready=true;
        bool bNodeExits=true;
        while(nd.idPrev!=idStartDrawNode)
        {

            pointPixel pt2;//=getCoordinate(hashNodes,nd.idPrev);
            if(bMoveAlready==true)
            {
                node ndNoUse;
                bNodeExits=getNode(nd.idPrev,ndNoUse);
                bool bFetch;
                node& ndTemp=fetchNodeFromQ0(nd.idPrev,bFetch);
                nd=ndTemp;
                if(bNodeExits==false)
                {
                    bMoveAlready=false;
                    continue;
                }
                else
                {

                    cairo_set_source_rgb(cr, 0,0,0);
                    cairo_set_line_width (cr, 1);
                    pt2=transformCoordinate(hashNodes,nd.id,scope);
                    cairo_line_to(cr,pt2.x,pt2.y);
                    pt1=pt2;
                    cairo_move_to(cr,pt2.x,pt2.y);
                    bMoveAlready=true;
                }
            }
            else
            {
                node ndNoUse;
                bNodeExits=getNode(nd.idPrev,ndNoUse);
                bool bFetch;
                node& ndTemp=fetchNodeFromQ0(nd.idPrev,bFetch);
                nd=ndTemp;
                if(bNodeExits==false)
                {
                    bMoveAlready=false;
                    continue;
                }
                else
                    {
                        cairo_set_source_rgb(cr, 0,0,0);
                        cairo_set_line_width (cr, 1);
                        pt2=transformCoordinate(hashNodes,nd.id,scope);
                        cairo_move_to(cr,pt2.x,pt2.y);
                        bMoveAlready=true;
                    }

            }
          



            cairo_stroke(cr);
        }

                    pointPixel pt2;//,pt1;
                    node ndNoUse;
                    bNodeExits=getNode(nd.id,ndNoUse);
                     cairo_set_source_rgb(cr, 0.1,0.8,0.2);
                    cairo_set_line_width (cr, 2);
                    if(bNodeExits==false)
                    {
                        continue;
                    }
                    else
                    {
                        pt1=transformCoordinate(hashNodes,nd.id,scope);
                        cairo_move_to(cr,pt1.x,pt1.y);
                    }
                    pt2=transformCoordinate(hashNodes,idStartDrawNode,scope);

                    cairo_line_to(cr,pt2.x,pt2.y);
                    cairo_stroke(cr);



    }
    cairo_surface_write_to_png(surface, "image.png");
    return true;
}

void improveListOfId(list<uint64_t>&lst)
{
    uint64_t id;//it=*lst.begin();

    for(auto it=lst.begin();it!=lst.end();)
    {
        id=*it;
        auto it2=it;
        it++;
        list<node>& lstNodes=hashNodes[id%SIZEHASH];
        auto itBar=lstNodes.begin();
        for(;itBar!=lstNodes.end();itBar++)
        {
            node& nd=*itBar;
            if(nd.id==id)
            {

                break;
            }
        }
        if(itBar==lstNodes.end())
        {
            lst.erase(it2);
        }

    }
}

void addEdgeFromList(list<uint64_t>& lst)
{
    if(lst.size()<2)
        return;
    for(auto it=lst.begin();it!=lst.end();it++)
    {
        auto itBar=it;
        itBar++;
        if(itBar==lst.end())
            break;
        auto id1=*it;
        auto id2=*itBar;
        addEdge(id1,id2,hashWays,hashNodes);
    }
    return;
}
void drawMap(latLonScope& scope)
{
    cairo_surface_t *surface;
    cairo_t *cr;
    surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,widthOfScreen,heightOfScreen);
    cr = cairo_create (surface);



    cairo_new_path (cr);
    cairo_rectangle(cr, 0, 0, widthOfScreen, heightOfScreen);
    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_fill(cr);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    /////////////////////////////////////////
    int nLoopHwaysCountLog=0;
    for(auto it=hashWays.begin();nLoopHwaysCountLog<SIZEHASHEDGES;it++,nLoopHwaysCountLog++)
    {
        list<wayNodes>& lstTemp=*it;
        pointPixel pt1,pt2;
        for(auto itSub=lstTemp.begin();itSub!=lstTemp.end();itSub++)
        {
            wayNodes& var=*itSub;
            uint64_t& idHead=var.id;
            pt1=transformCoordinate(hashNodes,idHead,scope);
            double radius1=0;
            double raduis2=360.0*(M_PI/180.0);

            cairo_set_source_rgb(cr, 0,0,0);
            cairo_set_line_width (cr, 1);
            cairo_move_to(cr,pt1.x,pt1.y);
            string strMark=to_string(idHead);

            list<uint64_t>& lstDeepSub=var.lstNodes;
            int nSize=lstDeepSub.size();
            int iLog=0;
            bool bFlagMoved=true;
            for(auto itDeepSub=lstDeepSub.begin();itDeepSub!=lstDeepSub.end();itDeepSub++)
            {
               uint64_t& id2=*itDeepSub;
               node ndNoUse;
               if(!getNode(id2,ndNoUse))
               {
                   bFlagMoved=false;
                   continue;

               }
               if(bFlagMoved==true)
               {
                   pt2=transformCoordinate(hashNodes,id2,scope);
                cairo_set_source_rgb(cr, 0,0,0);
                cairo_set_line_width (cr, 1);
               cairo_line_to(cr,pt2.x,pt2.y);
               cairo_move_to(cr,pt2.x,pt2.y);
               }
               else
                {
                    pt2=transformCoordinate(hashNodes,id2,scope);
                cairo_set_source_rgb(cr, 0,0,0);
                cairo_set_line_width (cr, 1);
               cairo_move_to(cr,pt2.x,pt2.y);
               bFlagMoved=true;
                }


               cairo_stroke(cr);
            }
        }
    }
     cairo_surface_write_to_png(surface, "imageMap.png");
}

bool checkListHasInt(int64_t id,list<uint64_t>&lst)
{
    for(auto it=lst.begin();it!=lst.end();it++)
    {
        if(*it==id)
        {
            return true;
        }
    }
    return false;

}
bool pushIntoQ0(node& nd)
{
    list<node>& lstNodes=vecQ0[nd.id%PRIME_HASH_SIZE];
    bool bFlag=true;
    node temp;
    for(auto it=lstNodes.begin();it!=lstNodes.end();it++)
    {
        temp=*it;
        if(temp.id==nd.id)
        {
           bFlag=false;
           break;
        }
    }
    if(bFlag==true)
    {
        lstNodes.push_back(nd);
    }
    return bFlag;
}
node& fetchNodeFromQ0(uint64_t id,bool& bFlagReturn)
{
     list<node>& lstNodes=vecQ0[id%PRIME_HASH_SIZE];
    bool bFlag=false;
    //node temp;
    auto it=lstNodes.begin();
     for(;it!=lstNodes.end();it++)
    {
        auto temp=*it;
        if(temp.id==id)
        {
           bFlag=true;
           //nd=temp;
           break;
        }
    }
    node ndFail;
    bFlagReturn=bFlag;
    if(bFlag==true)
    return *it;
    else
        return ndFail;

}
struct idAndSequenceId
{
    uint64_t id;
    int sequence;
    node nd;
};
struct latLonGradient
{
    double deltaLat;
    double deltaLon;
};
void addNewNodesToHashNodes(node ndNew)
{
    list<node>& lstNodes=hashNodes[ndNew.id%SIZEHASH];

    bool bFlag=true;
    node temp;
    for(auto it=lstNodes.begin();it!=lstNodes.end();it++)
    {
        temp=*it;
        if(temp.id==ndNew.id)
        {
           bFlag=false;
           break;
        }
    }
    if(bFlag==true)
    {
        lstNodes.push_back(ndNew);
    }
    return ;
}
void strengthListIds(list<uint64_t>&lst)
{

    if(lst.size()==0)
        return;
    else if(lst.size()==1)
        return;
    int nCount=0;
    list<idAndSequenceId> lstNodes;
    //list<idAndSequenceId> lstNodesToAdd;
    idAndSequenceId var1;
    int iItDistance=0;
    for(auto it=lst.begin();it!=lst.end();it++,iItDistance++)
    {
        uint64_t id=*it;
        node ndNoUse;
        if(true==getNode(id,ndNoUse))
            {
                nCount++;
                var1.id=id;
                var1.sequence=iItDistance;//(it-lst.begin());
                var1.nd=ndNoUse;
                lstNodes.push_back(var1);
            }

    }

    if(nCount<2)
        return;
    bool bHeadMissing=false;
    bool bEndMissing=false;
    auto itBar=lstNodes.begin();
    int iStartNumber=(*itBar).sequence;
    auto itLast=lstNodes.end();
    advance(itLast,-1);
    int iEndNumber=(*itLast).sequence;
    if(itBar->sequence!=0)
    {
        bHeadMissing=true;
    }
    if(iEndNumber!=lst.size()-1)
        bEndMissing=true;
    bool bHeadGradientFlag=false;
    bool bEndGradientFlag=false;
    latLonGradient headGradient;
    latLonGradient endGradient;
    latLonGradient gradient;
    for(;itBar!=lstNodes.end();itBar++)
    {

        idAndSequenceId& temp=*itBar;
        auto itBar2=itBar;
        itBar2++;
        if(itBar2==lstNodes.end())
        {
            break;
        }

        idAndSequenceId& temp2=*itBar2;
        int idDiff=temp2.sequence-temp.sequence;

        gradient.deltaLat=(temp2.nd.lat-temp.nd.lat)/idDiff;
        gradient.deltaLon=(temp2.nd.lon-temp.nd.lon)/idDiff;
        if(bHeadGradientFlag==false)
        {
            headGradient=gradient;
            bHeadGradientFlag=true;
        }
        if(idDiff==1)
            continue;
        else
        {
            int idSequence=temp.sequence+1;
            node ndTemp=temp.nd;
            while(idSequence<temp2.sequence)
            {
                auto itAdvance=lst.begin();
                advance(itAdvance,idSequence);
                auto itLst=itAdvance;//lst.begin()+idSequence;
                uint64_t idIndex=*itLst;
                node ndNew;
                ndNew.id=idIndex;
                ndNew.lat=gradient.deltaLat+ndTemp.lat;
                ndNew.lon=gradient.deltaLon+ndTemp.lon;
                addNewNodesToHashNodes(ndNew);
                ndTemp=ndNew;
                idSequence++;
            }
        }
    }
    endGradient=gradient;

    if(bHeadMissing)
    {
        idAndSequenceId& var=*lstNodes.begin();
        node& ndIndex=var.nd;
        for(auto index=iStartNumber-1;index>=0;index--)
        {
            auto itAdvance=lst.begin();
            advance(itAdvance,index);
            auto itLst=itAdvance;//lst.begin()+index;
            uint64_t idIndex=*itLst;
            node ndNew;
             ndNew.id=idIndex;
            ndNew.lat=ndIndex.lat-headGradient.deltaLat;
            ndNew.lon=ndIndex.lon-headGradient.deltaLon;
            addNewNodesToHashNodes(ndNew);
            ndIndex=ndNew;

        }
    }
    if(bEndMissing)
    {
        auto itAdvance=lstNodes.end();
        advance(itAdvance,-1);
        idAndSequenceId& var=*itAdvance;//*(lstNodes.end()-1);
        node& ndIndex=var.nd;
        for(auto index=iEndNumber+1;index<lst.size();index++)
        {
            auto itMove=lst.begin();
            advance(itMove,index);
            auto itLst=itMove;//lst.begin()+index;
            uint64_t idIndex=*itLst;
            node ndNew;
             ndNew.id=idIndex;
            ndNew.lat=ndIndex.lat+endGradient.deltaLat;
            ndNew.lon=ndIndex.lon+endGradient.deltaLon;
            addNewNodesToHashNodes(ndNew);
            ndIndex=ndNew;
        }

    }

}

void updateScope(list<node>lstEnds,uint64_t idStartDrawNode,latLonScope& scope)
{
    double latMin,latMax;
    double lonMin,lonMax;
    bool bInit=false;
    for (auto it=lstEnds.begin();it!=lstEnds.end();it++)
    {
        node nd=*it;
        nd.idPrev=(*it).idPrev;
        list<uint64_t> lstLineNodes;
        getLineToStartNode(nd.id,idStartDrawNode,lstLineNodes);
        for(auto it=lstLineNodes.begin();it!=lstLineNodes.end();it++)
        {
            node var;
            if(getNode(*it,var))
            {
                if(bInit==false)
                {
                    latMin=var.lat;
                    latMax=var.lat;
                    lonMin=var.lon;
                    lonMax=var.lon;
                }
                else
                {
                    if(latMin>var.lat)
                        latMin=var.lat;
                    if(latMax<var.lat)
                        latMax=var.lat;
                    if(lonMin>var.lon)
                        lonMin=var.lon;
                    if(lonMax<var.lon)
                        lonMax=var.lon;
                }
                bInit=true;
            }
        }
    }
    if(bInit==true)
    {
        double latDiff=latMax-latMin;
        double lonDiff=lonMax-lonMin;
        scope.minLat=latMin-latDiff/20;
        scope.maxLat=latMax+latDiff/20;
        scope.minLon=lonMin-lonDiff/20;
        scope.maxLon=lonMax+lonDiff/20;
        scope.latDiff=scope.maxLat-scope.minLat;
        scope.lonDiff=scope.maxLon-scope.minLon;

    }
}

