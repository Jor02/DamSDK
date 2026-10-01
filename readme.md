# DamSDK

DamSDK is a lightweight, independent synthesis plugin interface i'm developing alongside my [Delay Lama](https://github.com/Jor02/DelayLama) remake. 

I'm using knowledge I gained from having reverse engineered some old VST plugins, DamSDK provides an alternative to existing synthesis plugin frameworks. It is designed for developers who need high compatibility without the problems of legacy or proprietary SDKs that might no longer be available.

### Project Status
Currently, DamSDK is being developed as an integrated component of my Delay Lama remake project. As the DamSDK matures and the API stabilizes, it will be migrated to a dedicated standalone project for broader community use (if there's any interest for it that is).

### Structure
DamSDK follows the structure of the 2002-era VST SDK and VSTGUI 2.x that Delay Lama was built with, under its own names, so that it compiles to the same code as the original plugin:

| DamSDK | VST SDK / VSTGUI equivalent |
| :--- | :--- |
| `Api::AudioBase`, `Api::AudioBaseExtended` | `AudioEffect`, `AudioEffectX` |
| `Api::EditorInterface`, `Api::EditorBase` | `AEffEditor`, `AEffGUIEditor` |
| `Api::DamPlugin` | `AEffect` (same memory layout) |
| `Gui::Base::View`, `Gui::Controls::Control` | `CView`, `CControl` |
| `Gui::Platform::Windows::Window` | `CFrame` |
| `GDIDrawingContext`, `OffscreenGDIDrawingContext` | `CDrawContext`, `COffscreenContext` |
| `Bitmap`, `Rect`, `Point` | `CBitmap`, `CRect`, `CPoint` |
| `HorizontalSlider`, `VerticalSlider`, `RotaryControl`, `Knob` | `CHorizontalSlider`, `CVerticalSlider`, `CKnob`, `CAnimKnob` |
| `DropTarget` | `UDropTarget` |

It builds for 32-bit (including Visual C++ 6.0) and 64-bit Windows: the `DamPlugin` struct and event lists use the VST 2.4 64-bit layout (pointer-sized `value` and reserved fields).
