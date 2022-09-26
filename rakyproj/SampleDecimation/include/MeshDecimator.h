#pragma once
#include <queue>
#include <set>
#include <vector>
#include <limits>
#include "mdVector.h"
#include "mdSArray.h"


class UnstructuredGrid3D;


namespace MeshDecimation
{
    typedef float Float;
    struct MDVertex
    {
        SArray<int, SARRAY_DEFAULT_MIN_SIZE>    m_edges;    
        SArray<int, SARRAY_DEFAULT_MIN_SIZE>    m_triangles;    
        Float                                   m_Q[10];
                                                // 0 1 2 3
                                                //   4 5 6
                                                //     7 8
                                                //       9
        bool                                    m_tag;
        bool                                    m_onBoundary;
    };
    
    struct MDEdge
    {
        int                                     m_v1;
        int                                     m_v2;
        double                                  m_qem;
        Vec3<Float>                             m_pos;
        bool                                    m_onBoundary;
        bool                                    m_tag;
    };

    struct MDEdgePriorityQueue
    {
        int                                     m_name;
        double                                  m_qem;
        inline    friend bool                   operator<(const MDEdgePriorityQueue & lhs, const MDEdgePriorityQueue & rhs) { return (lhs.m_qem > rhs.m_qem);}
        inline    friend bool                   operator>(const MDEdgePriorityQueue & lhs, const MDEdgePriorityQueue & rhs) { return (lhs.m_qem < rhs.m_qem);}
    };

    typedef void (*CallBackFunction)(const char * msg);

    class MeshDecimator
    {
    public:

        void                                    SetCallBack(CallBackFunction  callBack) { m_callBack = callBack;}
        const CallBackFunction                  GetCallBack() const { return m_callBack;}
        inline void                             SetEColManifoldConstraint(bool ecolManifoldConstraint) { m_ecolManifoldConstraint = ecolManifoldConstraint; }
        inline size_t                           GetNVertices()const {return m_nVertices;};
        inline size_t                           GetNTriangles() const {return m_nTriangles;};
        inline size_t                           GetNEdges() const {return m_nEdges;};
        void                                    GetMeshData(Vec3<Float> * points, Vec3<int> * triangles) const;
        void					GetDecimatedOutput(UnstructuredGrid3D*&);
        void                                    ReleaseMemory();
        void 					Initialize(const UnstructuredGrid3D&);
        void                                    Initialize(size_t nVertices, size_t nTriangles, 
                                                           Vec3<Float> *  points, 
                                                           Vec3<int> * triangles);        
        bool                                    Decimate(size_t targetNVertices = 100, 
                                                         size_t targetNTriangles = 0, 
                                                         double targetError = std::numeric_limits<double>::max());

                                                MeshDecimator(void);
                                                ~MeshDecimator(void);
    private : 
        void                                    EdgeCollapse(int v1, int v2);
        int                                     GetTriangle(int v1, int v2, int v3) const;
        int                                     GetEdge(int v1, int v2) const;
        int                                     IsBoundaryEdge(int v1, int v2) const;
        bool                                    IsBoundaryVertex(int v) const;
        void                                    InitializePriorityQueue();
        void                                    InitializeQEM();
        bool                                    ManifoldConstraint(int v1, int v2) const;
        double                                  ComputeEdgeCost(int v1, int v2, Vec3<Float> & pos) const;
        bool                                    EdgeCollapse(double & error);
    
        Vec3<int> *                             m_triangles;
        Vec3<Float> *                           m_points;
        size_t                                  m_nPoints;
        size_t                                  m_nInitialTriangles;
        size_t                                  m_nVertices;
        size_t                                  m_nTriangles;
        size_t                                  m_nEdges;
        double                                  m_diagBB;
        std::vector<MDVertex>                   m_vertices;
        std::vector<MDEdge>                     m_edges;
        std::priority_queue<
             MDEdgePriorityQueue, 
             std::vector<MDEdgePriorityQueue>, 
             std::less<MDEdgePriorityQueue> >   m_pqueue;
        CallBackFunction                        m_callBack;                    //>! call-back function
        bool *                                  m_trianglesTags;
        bool                                    m_ecolManifoldConstraint;
        std::vector< Vec3<Float> >		decPoints;
        std::vector< Vec3<int> > 		decTriangles;
    };
}
