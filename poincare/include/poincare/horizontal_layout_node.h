#ifndef POINCARE_HORIZONTAL_LAYOUT_NODE_H
#define POINCARE_HORIZONTAL_LAYOUT_NODE_H

#include "layout_reference.h"
#include "layout_node.h"
#include "layout_cursor.h"

namespace Poincare {

/* WARNING: A HorizontalLayout should never have a HorizontalLayout child. For
 * instance, use addOrMergeChildAtIndex to add a LayoutNode safely. */

class HorizontalLayoutNode : public LayoutNode {
  friend class LayoutReference;
public:
  using LayoutNode::LayoutNode;

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  LayoutCursor equivalentCursor(LayoutCursor * cursor) override;
  void deleteBeforeCursor(LayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override;

  bool isHorizontal() const override { return true; }
  bool isEmpty() const override { return numberOfChildren() == 1 && const_cast<HorizontalLayoutNode *>(this)->childAtIndex(0)->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const override { return numberOfChildren() != 0; }

  // TreeNode
  size_t size() const override { return sizeof(HorizontalLayoutNode); }
#if TREE_LOG
  const char * description() const override {
    return "Horizontal Layout";
  }
#endif

protected:
  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  bool willAddChildAtIndex(LayoutNode * l, int * index, LayoutCursor * cursor) override;
  bool willAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor) override;
  bool willRemoveChild(LayoutNode * l, LayoutCursor * cursor, bool force) override;
  void didRemoveChildAtIndex(int index, LayoutCursor * cursor, bool force) override;
  bool willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) override;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class HorizontalLayoutRef : public LayoutReference {
  friend class HorizontalLayoutNode;
public:
  HorizontalLayoutRef(TreeNode * n) : LayoutReference(n) {}
  HorizontalLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<HorizontalLayoutNode>();
    m_identifier = node->identifier();
  }

  HorizontalLayoutRef(LayoutRef l) : HorizontalLayoutRef() {
    addChildTreeAtIndex(l, 0);
  }

  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2) : HorizontalLayoutRef() {
    addChildTreeAtIndex(l1, 0);
    addChildTreeAtIndex(l2, 1);
  }
  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3) : HorizontalLayoutRef() {
    addChildTreeAtIndex(l1, 0);
    addChildTreeAtIndex(l2, 1);
    addChildTreeAtIndex(l3, 2);
  }
  HorizontalLayoutRef(LayoutRef l1, LayoutRef l2, LayoutRef l3, LayoutRef l4) : HorizontalLayoutRef() {
    addChildTreeAtIndex(l1, 0);
    addChildTreeAtIndex(l2, 1);
    addChildTreeAtIndex(l3, 2);
    addChildTreeAtIndex(l4, 3);
  }

  void addOrMergeChildAtIndex(LayoutRef l, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);
  void mergeChildrenAtIndex(HorizontalLayoutRef h, int index, bool removeEmptyChildren, LayoutCursor * cursor = nullptr);
private:
  int removeEmptyChildBeforeInsertionAtIndex(int index, bool shouldRemoveOnLeft, LayoutCursor * cursor = nullptr);
};

}

#endif
