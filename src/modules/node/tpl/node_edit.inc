<h2>[TEXT:Edit Content]</h2>
[SHADOW_DESCRIPTION:Here you can edit this node's title and contents.]
[FORM|name=shadow-form|id=shadow-form:
[INPUT|title=title|id=edit_title|required|name=edit-title|value={ATTRIBUTE|name=title|no_qtags}:shadow-form]
[INPUT|title=teaser|id=edit_teaser|rows=2|name=edit-teaser|type=text|value={ATTRIBUTE|name=teaser|no_qtags}:shadow-form]
[INPUT|wysiwyg=true|title=body|id=edit_content|required|name=edit-content|type=text|value={ATTRIBUTE|name=body|no_qtags}:shadow-form]
]