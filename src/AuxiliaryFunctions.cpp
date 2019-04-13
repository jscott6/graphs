
#include "AuxiliaryFunctions.h"
using namespace std;
using namespace Rcpp;
using IV = Rcpp::IntegerVector;
using IM = Rcpp::IntegerMatrix;

// prints a 2-d array to output stream
void printMatrix(const IM vec)
{
  int n = vec.nrow();
  Rcout << endl;
  for (int i = 0; i < n; i++)
  {
    int m = vec.ncol();
    for (int j = 0; j < m; j++)
    {
      Rcout << setprecision(2) << setw(4);
      Rcout << vec(i, j) << " ";
    }
    Rcout << endl;
  }
  Rcout << endl;
}

// This if for debugging purposes

void printEdgeData(const Weighted::Edge &e)
{
  Rcout << "Edge: " << e.tail_->index + 1 << "->" << e.head_->index + 1 << endl;
  Rcout << "Weight: " << e.weight() << endl;
  Rcout << "even: " << e.even() << " odd: " << e.odd() << endl;
  Rcout << endl;
}

void printEdgeData(const Unweighted::Edge &e)
{
  Rcout << "Edge: " << e.tail_->index + 1 << "->" << e.head_->index + 1 << endl;
  Rcout << "Weight: " << e.weight() << endl;
  Rcout << endl;
}


void printVertexData(const Weighted::Vertex &v)
{
  Rcout << "------------------------" << endl;
  Rcout << "VERTEX " << v.index + 1 << endl;
  Rcout << "------------------------" << endl;
  Rcout << endl;
  Rcout << setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->tail_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->head_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->weight() << " ";
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Possible Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->tail_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->head_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Possible Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << "visits: " << v.cycle_data.visits << endl;
  Rcout << "up_zero_visits: " << v.cycle_data.up_zero_visits << endl;
  Rcout << "low_zero_visits: " << v.cycle_data.low_zero_visits << endl;
  Rcout << "up_zeros: " << v.cycle_data.up_zeros << endl;
  Rcout << "low_zeros: " << v.cycle_data.low_zeros << endl;
  return;
}



void printVertexData(const Unweighted::Vertex &v)
{
  Rcout << "------------------------" << endl;
  Rcout << "VERTEX " << v.index + 1 << endl;
  Rcout << "------------------------" << endl;
  Rcout << endl;
  Rcout << setw(8) << "In Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->tail_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->head_->index + 1 << " ";
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.in_edges.size(); ++i)
    Rcout << setw(2) << v.in_edges[i]->weight() << " ";
  Rcout << endl;
  Rcout << endl;
  Rcout << setw(8) << "Not Out Edges" << endl;
  Rcout << "------------" << endl;
  Rcout << setw(8) << "Tail: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->tail_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Head: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->head_->index + 1 << " ";
  }
  Rcout << endl;
  Rcout << setw(8) << "Weight: ";
  for (int i = 0; i != v.not_out_edges.size(); ++i)
  {
    Rcout << setw(2) << v.not_out_edges[i]->weight() << " ";
  }
  Rcout << endl;
  Rcout << endl;
  return;
}

// [[Rcpp::export]]
IM constructGraph(IV out_degree, IV in_degree, IM fixed)
{
  checkInputs(in_degree, out_degree, fixed);
  bool sinkfound = true;
  ConstructGraph::Graph net(out_degree, in_degree, fixed);
  // adjust flow until no path remains in the residual Graph
  while (sinkfound) {
    sinkfound = net.findPath();
    net.updateFlow(net.calcPathFlow());
  }
  return net.constructWeightMatrix();
}

void checkInputs(IV in_degree, IV out_degree, IM fixed) {
  if(in_degree.size() != fixed.nrow() || out_degree.size() != fixed.ncol())
    throw invalid_argument("Degree vectors and fixed matrix of incompatible dimensions");
  if(min(out_degree) < 0 || min(in_degree) < 0)
    throw invalid_argument("Degree vectors must be non-negative");
  if(min(fixed) < 0 || max(fixed) > 1)
    throw invalid_argument("All entries of fixed matrix must be binary valued");
}






