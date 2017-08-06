

class tree_node {
    _children:                                  list<&tree_node>;
    
    fn __alwyas_inline __child_as<T>(const &self, i: usize) -> &T where is<T, tree_node> {
        ret dyn_cast<&T>(self._children[i]);
    }
    
    fn __always_inline __append_child<T>(const &self, in child: &T) -> usize where is<T, tree_node> {
        self._children.append(child);
        ret self._children.size - 1;
    }
}

descriptor property_node<T> {
    _index:                                     const usize;
    
    fn .ctor(&self, parent: &tree_node) {
        
    }
    
    fn inline .get(&self, parent: &tree_node) -> &T {
        ret parent._children[i];
    }
}
