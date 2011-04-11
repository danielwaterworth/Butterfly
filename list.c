/*  
    Copyright (C) 2011 Butterfly authors,
    
    This file is part of Butterfly.
    
    Butterfly is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Butterfly is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "assert.h"
#include "stdlib.h"

#include "list.h"

static list *create_leaf(object *obj) {
    list *result = malloc(sizeof(list));
    result->type = LEAF;
    result->data.leaf = object_copy(obj);
    return result;
}

static list *free_leaf(list *leaf) {
    assert(leaf->type == LEAF);
    object_free(leaf->data.leaf);
    free(leaf);
    return NULL;
}

static int32_t node_length(list *l) {
    if (l->type == LEAF) {
        return 1;
    }
    return l->data.branch.nodes;
}

int32_t list_length(finger_branch *branch) {
    return branch->nodes;
}

static void list_ptr_set(list **child, int32_t i, object *obj) {
    if ((*child)->type == LEAF) {
        assert(i < 2 && i >= 0);
        if (i == 0) {
            object_free((*child)->data.leaf);
            (*child)->data.leaf = object_copy(obj);
        } else {
            list *branch = malloc(sizeof(list));
            branch->type = BRANCH;
            branch->data.branch.left = *child;
            branch->data.branch.right = create_leaf(obj);
            branch->data.branch.nodes = 2;
            *child = branch;
        }
    } else {
        list_set(&(*child)->data.branch, i, obj);
    }
}

void list_set(finger_branch *branch, int32_t i, object *obj) {
    assert(i <= branch->nodes && i >= 0);
    if (branch->nodes == 0) {
        branch->left = create_leaf(obj);
    } else {
        int32_t left_len = node_length(branch->left);
        if (i >= left_len) {
            if (branch->right != NULL) {
                list_ptr_set(&branch->right, i - left_len, obj);
            } else {
                branch->right = create_leaf(obj);
            }
        } else {
            list_ptr_set(&branch->left, i, obj);
        }
    }
    if (i == branch->nodes) {
        branch->nodes += 1;
    }
}

static object *list_ptr_get(list *child, int32_t i) {
    if (child->type == LEAF) {
        return object_copy(child->data.leaf);
    } else {
        return list_get(&child->data.branch, i);
    }
}

object *list_get(finger_branch *branch, int32_t i) {
    if (i < 0 || i >= branch->nodes) {
        return NULL;
    }
    int32_t left_len = node_length(branch->left);
    if (i >= left_len) {
        i -= left_len;
        return list_ptr_get(branch->right, i);
    } else {
        return list_ptr_get(branch->left, i);
    }
}

static void list_ptr_insert_at(list **l, int32_t i, object *obj) {
    if (*l == NULL) {
        *l = create_leaf(obj);
    } else {
        if ((*l)->type == BRANCH) {
            list_insert_at(&(*l)->data.branch, i, obj);
        } else {
            list *leaf = *l;
            *l = malloc(sizeof(list));
            (*l)->type = BRANCH;
            (*l)->data.branch.nodes = 2;
            (*l)->data.branch.left = create_leaf(obj);
            (*l)->data.branch.right = leaf;
        }
    }
}

void list_insert_at(finger_branch *branch, int32_t i, object *obj) {
    assert (i >= 0 && i <= branch->nodes);
    if (branch->nodes == 0) {
        branch->left = create_leaf(obj);
    } else {
        if (i == 0) {
            list_ptr_insert_at(&branch->left, i, obj);
        } else {
            int32_t left_len = node_length(branch->left);
            if (i >= left_len) {
                list_ptr_insert_at(&branch->right, i-left_len, obj);
            } else {
                list_ptr_insert_at(&branch->left, i, obj);
            }
        }
    }
    branch->nodes += 1;
}

static list *list_ptr_remove(list *l, int32_t i) {
    if (l->type == LEAF) {
        free_leaf(l);
        return NULL;
    } else {
        int32_t left_len = node_length(l->data.branch.left);
        if (i >= left_len) {
            i -= left_len;
            l->data.branch.right = list_ptr_remove(l->data.branch.right, i);
            if (l->data.branch.right == NULL) {
                list *ret = l->data.branch.left;
                free(l);
                return ret;
            }
        } else {
            l->data.branch.left = list_ptr_remove(l->data.branch.left, i);
            if (l->data.branch.left == NULL) {
                list *ret = l->data.branch.right;
                free(l);
                return ret;
            }
        }
        l->data.branch.nodes -= 1;
    }
    return NULL;
}

void list_remove(finger_branch *branch, int32_t i) {
    assert(i >= 0 && i < branch->nodes);
    if (branch->nodes == 0) {
        return;
    }
    int32_t left_len = node_length(branch->left);
    if (i >= left_len) {
        branch->right = list_ptr_remove(branch->right, i-left_len);
    } else {
        branch->left = list_ptr_remove(branch->left, i);
        if (branch->left == NULL) {
            branch->left = branch->right;
            branch->right = NULL;
        }
    }
    branch->nodes -= 1;
}

list *list_copy(list *l) {
    if (l == NULL) {
        return NULL;
    }
    list *res = malloc(sizeof(list));
    res->type = l->type;
    if (l->type == LEAF) {
        res->data.leaf = object_copy(l->data.leaf);
    } else {
        res->data.branch.nodes = l->data.branch.nodes;
        res->data.branch.left = list_copy(l->data.branch.left);
        res->data.branch.right = list_copy(l->data.branch.right);
    }
    return res;
}
