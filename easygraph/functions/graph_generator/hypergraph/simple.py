from itertools import combinations

from easygraph.classes.hypergraph import Hypergraph
from easygraph.utils.exception import EasyGraphError


__all__ = [
    "star_clique",
    "sunflower",
]


def star_clique(n_star, n_clique, d_max):
    """Generate a star-clique structure

    That is a star network and a clique network,
    connected by one pairwise edge connecting the centre of the star to the clique.
    network, the each clique is promoted to a hyperedge
    up to order d_max.

    Parameters
    ----------
    n_star : int
        Number of legs of the star
    n_clique : int
        Number of nodes in the clique
    d_max : int
        Maximum order up to which to promote
        cliques to hyperedges

    Returns
    -------
    H : Hypergraph

    Examples
    --------
    >>> import easygraph as eg
    >>> H = eg.star_clique(6, 7, 2)

    Notes
    -----
    The total number of nodes is n_star + n_clique.

    """

    if n_star <= 0:
        raise ValueError("n_star must be an integer > 0.")
    if n_clique <= 0:
        raise ValueError("n_clique must be an integer > 0.")
    if d_max < 0:
        raise ValueError("d_max must be an integer >= 0.")
    elif d_max > n_clique - 1:
        raise ValueError("d_max must be <= n_clique - 1.")

    nodes_star = range(n_star)
    nodes_clique = range(n_star, n_star + n_clique)
    nodes = list(nodes_star) + list(nodes_clique)

    H = Hypergraph(num_v=len(nodes))

    # add star edges (center of the star is 0-th node)
    H.add_hyperedges([[nodes_star[0], nodes_star[i]] for i in range(1, n_star)])

    # connect clique and star by adding last star leg
    H.add_hyperedges([nodes_star[0], nodes_clique[0]])

    # add clique hyperedges up to order d_max

    H.add_hyperedges(
        [
            e
            for d in range(1, d_max + 1)
            for e in list(combinations(nodes_clique, d + 1))
        ]
    )

    return H


def sunflower(l, c, m):
    """Create a sunflower hypergraph.

    This creates an m-uniform hypergraph
    according to the sunflower model.

    Parameters
    ----------
    l : int
        Number of petals
    c : int
        Size of the core
    m : int
        Size of each edge

    Raises
    ------
    EasyGraphError
        If the edge size is smaller than the core.

    Returns
    -------

    """

    if m < c:
        raise EasyGraphError("m cannot be smaller than c.")

    core_nodes = list(range(c))

    H = Hypergraph(num_v=len(core_nodes))
    start_label = c
    while start_label + (m - c) <= c + (m - c) * l:
        H.add_hyperedges(core_nodes + [start_label + i for i in range(m - c)])
        start_label = start_label + (m - c)

    return H
