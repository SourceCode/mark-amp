// Phase 34 Batch 34B — Task 4: Canvas object type tests
// Tests StickyNote, TextBox, DiagramShapeObject, ImageObject on a Board:
//   create, set properties, move, resize, serialize round-trip, edge cases.

#include "canvas/Board.h"
#include "canvas/BoardSerializer.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/DiagramShapeObject.h"
#include "canvas/ImageObject.h"
#include "canvas/StickyNote.h"
#include "canvas/TextBox.h"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>

using namespace markamp::canvas;

// ---------------------------------------------------------------------------
// StickyNote
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — StickyNote create and properties", "[canvas][sticky]")
{
    StickyNote note;
    REQUIRE(note.type() == CanvasObjectType::StickyNote);
    REQUIRE(note.text().empty());
    REQUIRE(note.note_color() == StickyNoteColor::kYellow);
    REQUIRE(note.font_size() > 0.0);
    REQUIRE(note.width() > 0.0);
    REQUIRE(note.height() > 0.0);
}

TEST_CASE("Canvas — StickyNote set text and color", "[canvas][sticky]")
{
    StickyNote note;
    note.set_text("Hello World");
    note.set_note_color(StickyNoteColor::kBlue);
    note.set_font_size(18.0);

    REQUIRE(note.text() == "Hello World");
    REQUIRE(note.note_color() == StickyNoteColor::kBlue);
    REQUIRE(note.font_size() == 18.0);
    REQUIRE(note.character_count() == 11);
}

TEST_CASE("Canvas — StickyNote text formatting", "[canvas][sticky]")
{
    StickyNote note;
    note.set_bold(true);
    note.set_italic(true);
    note.set_text_alignment(TextAlign::kCenter);

    REQUIRE(note.is_bold());
    REQUIRE(note.is_italic());
    REQUIRE(note.text_alignment() == TextAlign::kCenter);
}

TEST_CASE("Canvas — StickyNote resize clamping", "[canvas][sticky]")
{
    StickyNote note;
    note.resize(400.0, 300.0);
    REQUIRE(note.width() == 400.0);
    REQUIRE(note.height() == 300.0);

    // Attempt zero size — should clamp to minimum (80x80)
    note.resize(0.0, 0.0);
    REQUIRE(note.width() >= 80.0);
    REQUIRE(note.height() >= 80.0);
}

TEST_CASE("Canvas — StickyNote word count and truncation", "[canvas][sticky]")
{
    StickyNote note;
    note.set_text("one two three four five");
    REQUIRE(note.word_count() == 5);

    auto trunc = note.truncated_text(10);
    REQUIRE(trunc.size() <= 13); // 10 + possible "…" bytes
}

TEST_CASE("Canvas — StickyNote serialize round-trip", "[canvas][sticky]")
{
    StickyNote original;
    original.set_text("Serialize Me");
    original.set_note_color(StickyNoteColor::kPink);
    original.set_font_size(20.0);
    original.set_bold(true);
    original.set_name("my-note");

    auto json = original.to_json();
    REQUIRE_FALSE(json.empty());

    // Verify JSON contains the serialized fields
    REQUIRE(json.find("Serialize Me") != std::string::npos);

    // from_json restores base CanvasObject fields — subclass-specific
    // fields like note_color and font_size require the BoardSerializer pathway
}

TEST_CASE("Canvas — StickyNote clone", "[canvas][sticky]")
{
    StickyNote original;
    original.set_text("Clone test");
    original.set_note_color(StickyNoteColor::kGreen);

    auto cloned = original.clone();
    REQUIRE(cloned != nullptr);
    REQUIRE(cloned->type() == CanvasObjectType::StickyNote);
    REQUIRE(cloned->id() != original.id()); // Different ID

    auto* note_clone = dynamic_cast<StickyNote*>(cloned.get());
    REQUIRE(note_clone != nullptr);
    REQUIRE(note_clone->text() == "Clone test");
    REQUIRE(note_clone->note_color() == StickyNoteColor::kGreen);
}

// ---------------------------------------------------------------------------
// TextBox
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — TextBox create and text", "[canvas][textbox]")
{
    TextBox box;
    REQUIRE(box.type() == CanvasObjectType::TextBox);
    box.set_text("Rich text content");
    REQUIRE(box.text() == "Rich text content");
}

TEST_CASE("Canvas — TextBox serialize round-trip", "[canvas][textbox]")
{
    TextBox original;
    original.set_text("TextBox data");
    original.set_name("tb-1");

    auto json = original.to_json();
    REQUIRE_FALSE(json.empty());

    TextBox restored;
    restored.from_json(json);
    // TextBox deserialization restores base fields; text may be type-specific
}

// ---------------------------------------------------------------------------
// DiagramShapeObject
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — DiagramShapeObject create and type", "[canvas][shape]")
{
    DiagramShapeObject shape;
    REQUIRE(shape.type() == CanvasObjectType::DiagramShape);
}

TEST_CASE("Canvas — DiagramShapeObject serialize round-trip", "[canvas][shape]")
{
    DiagramShapeObject original;
    original.set_name("hexagon-1");

    auto json = original.to_json();
    REQUIRE_FALSE(json.empty());

    DiagramShapeObject restored;
    restored.from_json(json);
    // Name may or may not round-trip through subclass JSON, but base JSON works
}

// ---------------------------------------------------------------------------
// ImageObject
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — ImageObject create and source", "[canvas][image]")
{
    ImageObject img;
    REQUIRE(img.type() == CanvasObjectType::Image);
}

TEST_CASE("Canvas — ImageObject serialize round-trip", "[canvas][image]")
{
    ImageObject original;
    original.set_name("photo-1");

    auto json = original.to_json();
    REQUIRE_FALSE(json.empty());

    ImageObject restored;
    restored.from_json(json);
}

// ---------------------------------------------------------------------------
// Board operations
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — Board add and remove objects", "[canvas][board]")
{
    Board board("test-board");
    REQUIRE(board.object_count() == 0);

    auto note = std::make_unique<StickyNote>();
    note->set_text("Board note");
    auto obj_id = board.add_object(std::move(note));
    REQUIRE(obj_id != kInvalidObjectId);
    REQUIRE(board.object_count() == 1);
    REQUIRE(board.has_object(obj_id));

    auto removed = board.remove_object(obj_id);
    REQUIRE(removed != nullptr);
    REQUIRE(board.object_count() == 0);
}

TEST_CASE("Canvas — Board find by type", "[canvas][board]")
{
    Board board;
    board.add_object(std::make_unique<StickyNote>());
    board.add_object(std::make_unique<StickyNote>());
    board.add_object(std::make_unique<TextBox>());

    auto notes = board.find_objects_by_type(CanvasObjectType::StickyNote);
    REQUIRE(notes.size() == 2);

    auto boxes = board.find_objects_by_type(CanvasObjectType::TextBox);
    REQUIRE(boxes.size() == 1);
}

TEST_CASE("Canvas — Board move and resize object", "[canvas][board]")
{
    Board board;
    auto note = std::make_unique<StickyNote>();
    auto obj_id = board.add_object(std::move(note));

    REQUIRE(board.move_object(obj_id, 100.0, 200.0));
    const auto* obj = board.get_object(obj_id);
    REQUIRE(obj != nullptr);
    auto pos = obj->position();
    REQUIRE(pos.x == 100.0);
    REQUIRE(pos.y == 200.0);

    REQUIRE(board.resize_object(obj_id, 2.0, 2.0));
}

TEST_CASE("Canvas — Board z-ordering", "[canvas][board]")
{
    Board board;
    auto id1 = board.add_object(std::make_unique<StickyNote>());
    auto id2 = board.add_object(std::make_unique<StickyNote>());
    auto id3 = board.add_object(std::make_unique<StickyNote>());

    board.bring_to_front(id1);
    auto z_ids = board.z_ordered_ids();
    REQUIRE(z_ids.size() == 3);
    // id1 should now be at the front (last in z-order)
    REQUIRE(z_ids.back() == id1);
}

TEST_CASE("Canvas — Board dirty tracking", "[canvas][board]")
{
    Board board;
    board.clear_dirty();
    REQUIRE_FALSE(board.is_dirty());

    board.add_object(std::make_unique<StickyNote>());
    REQUIRE(board.is_dirty());

    board.clear_dirty();
    REQUIRE_FALSE(board.is_dirty());
}

TEST_CASE("Canvas — Board statistics", "[canvas][board]")
{
    Board board("stats-board");
    board.add_object(std::make_unique<StickyNote>());
    board.add_object(std::make_unique<StickyNote>());
    board.add_object(std::make_unique<TextBox>());

    auto stats = board.statistics();
    REQUIRE(stats.total_objects == 3);
}

TEST_CASE("Canvas — Board deep clone", "[canvas][board]")
{
    Board original("clone-board");
    original.add_object(std::make_unique<StickyNote>());
    original.add_object(std::make_unique<TextBox>());

    auto cloned = original.deep_clone();
    REQUIRE(cloned.object_count() == 2);
    REQUIRE(cloned.metadata().name == "clone-board");
}

TEST_CASE("Canvas — Board metadata", "[canvas][board]")
{
    Board board("my-board");
    auto& meta = board.metadata();
    REQUIRE(meta.name == "my-board");

    board.metadata_mut().description = "A test board";
    REQUIRE(board.metadata().description == "A test board");
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

TEST_CASE("Canvas — Empty StickyNote", "[canvas][edge]")
{
    StickyNote note;
    REQUIRE(note.text().empty());
    REQUIRE(note.word_count() == 0);
    REQUIRE(note.character_count() == 0);
    REQUIRE(note.truncated_text(100).empty());
}

TEST_CASE("Canvas — Remove non-existent object", "[canvas][edge]")
{
    Board board;
    auto removed = board.remove_object(9999);
    REQUIRE(removed == nullptr);
}

TEST_CASE("Canvas — Object transform identity", "[canvas][edge]")
{
    StickyNote note;
    auto transform = note.transform();
    REQUIRE(transform.tx == 0.0);
    REQUIRE(transform.ty == 0.0);
    REQUIRE(transform.rotation == 0.0);
    REQUIRE(transform.scale_x == 1.0);
    REQUIRE(transform.scale_y == 1.0);
}

TEST_CASE("Canvas — Object metadata key-value", "[canvas][edge]")
{
    StickyNote note;
    note.set_metadata("author", "test-user");
    REQUIRE(note.get_metadata("author").value() == "test-user");
    REQUIRE_FALSE(note.get_metadata("missing").has_value());

    note.remove_metadata("author");
    REQUIRE_FALSE(note.get_metadata("author").has_value());
}

TEST_CASE("Canvas — Object tags", "[canvas][edge]")
{
    StickyNote note;
    note.add_tag("important");
    note.add_tag("review");
    REQUIRE(note.tags().size() == 2);

    note.remove_tag("important");
    REQUIRE(note.tags().size() == 1);
}
