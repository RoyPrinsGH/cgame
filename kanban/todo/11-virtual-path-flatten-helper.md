# Home for the virtual-path-to-string helper (companion to graphics #06)

`shader_controller::keyOf` and `model_controller::keyOf` in cgame-graphics
both hand-roll a join of `virtual_asset_path::pathParts()` with '/'. Ticket
06 in the graphics kanban wants one method on `virtual_asset_path` to replace
both, but that type lives here in cgame-assets, which is not part of the
graphics extraction.

This is that side of the change: add the method (name it, e.g.
`flattened()` or `key()`) to `virtual_asset_path` in pak.hpp, so the graphics
controllers can drop their duplicated joins and call it instead.

Touches: pak.hpp.
