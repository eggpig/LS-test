import itertools

from easygraph.classes.hypergraph import Hypergraph
from easygraph.utils.exception import EasyGraphError


__all__ = ["empty_hypergraph", "complete_hypergraph"]


def empty_hypergraph(N=1):
    """

    Parameters
    ----------
    N number of node in Hypergraph, default 1

    Returns
    -------
    A eg.Hypergraph with n_num node, without any hyperedge.

    """
    return Hypergraph(N)


def complete_hypergraph(n, include_singleton=False):
    if n == 0:
        raise EasyGraphError("The number of nodes in a Hypergraph can not be zero")
    # init
    hypergraph = Hypergraph(n)
    total_hyperedegs = []
    if n > 1:
        start = 1 if include_singleton else 2
        for size in range(start, n + 1):
            hyperedges = itertools.combinations(list(range(n)), size)
            total_hyperedegs.extend(list(hyperedges))
        hypergraph.add_hyperedges(total_hyperedegs)
    return hypergraph
