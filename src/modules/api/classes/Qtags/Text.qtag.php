<?php
namespace Quanta\Qtags;

/**
 * Renders a simple (translatable) string.
 */

class Text extends Qtag {
  /**
   * Render the Qtag.
   *
   * @return text
   *   The rendered Qtag.
   */
  public function render() {
    return t($this->getTarget());
  }
}
