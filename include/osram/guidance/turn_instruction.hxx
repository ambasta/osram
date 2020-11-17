#ifndef OSRAM_GUIDANCE_TURN_INSTUCTION
#define OSRAM_GUIDANCE_TURN_INSTUCTION

#include <cstdint>
#include <osram/guidance/roundabout_type.hxx>
#include <osram/util/typedefs.hxx>

namespace osram {
namespace guidance {
enum DirectionModifier {
  UTurn = 0,
  SharpRight = 1,
  Right = 2,
  SlightRight = 3,
  Straight = 4,
  SlightLeft = 5,
  Left = 6,
  SharpLeft = 7,
  MaxDirectionModifier = 8,
};

enum TurnType {
  Invalid = 0,
  NewName = 1,
  Continue = 2,
  Turn = 3,
  Merge = 4,
  OnRamp = 5,
  OffRamp = 6,
  Fork = 7,
  EndOfRoad = 8,
  Notification = 9,
  EnterRoundabout = 10,
  EnterAndExitRoundabout = 11,
  EnterRotary = 12,
  EnterAndExitRotary = 13,
  EnterRoundaboutIntersection = 14,
  EnterAndExitRoundaboutIntersection = 15,

  NoTurn = 17,
  Suppressed = 18,
  EnterRoundaboutAtExit = 19,
  ExitRoundabout = 20,
  EnterRotaryAtExit = 21,
  ExitRotary = 22,
  EnterRoundaboutIntersectionAtExit = 23,
  ExitRoundaboutIntersection = 24,
  StayOnRoundabout = 25,
  Sliproad = 26,

  MaxTurnType = 27,
};

struct TurnInstruction {
  TurnType type;
  DirectionModifier direction_modifier;

  TurnInstruction(
      const TurnType type = TurnType::Invalid,
      const DirectionModifier direction_modifier = DirectionModifier::UTurn)
      : type(type), direction_modifier(direction_modifier) {}

  bool is_uturn() const {
    return type != TurnType::NoTurn &&
           direction_modifier == DirectionModifier::UTurn;
  }

  static TurnInstruction INVALID() {
    return {TurnType::Invalid, DirectionModifier::UTurn};
  }

  static TurnInstruction NO_TURN() {
    return {TurnType::NoTurn, DirectionModifier::UTurn};
  }

  static TurnInstruction REMAIN_ROUNDABOUT(const RoundaboutType,
                                           const DirectionModifier modifier) {
    return {TurnType::StayOnRoundabout, modifier};
  }

  static TurnInstruction ENTER_ROUNDABOUT(const RoundaboutType roundabout_type,
                                          const DirectionModifier modifier) {
    const constexpr TurnType enter_instruction[] = {
        TurnType::Invalid, TurnType::EnterRoundabout, TurnType::EnterRotary,
        TurnType::EnterRoundaboutIntersection};
    return {enter_instruction[static_cast<int>(roundabout_type)], modifier};
  }

  static TurnInstruction EXIT_ROUNDABOUT(const RoundaboutType roundabout_type,
                                         const DirectionModifier modifier) {
    const constexpr TurnType exit_instruction[] = {
        TurnType::Invalid, TurnType::ExitRoundabout, TurnType::ExitRotary,
        TurnType::ExitRoundaboutIntersection};
    return {exit_instruction[static_cast<int>(roundabout_type)], modifier};
  }

  static TurnInstruction
  ENTER_AND_EXIT_ROUNDABOUT(const RoundaboutType roundabout_type,
                            const DirectionModifier modifier) {
    const constexpr TurnType exit_instruction[] = {
        TurnType::Invalid, TurnType::EnterAndExitRoundabout,
        TurnType::EnterAndExitRotary,
        TurnType::EnterAndExitRoundaboutIntersection};
    return {exit_instruction[static_cast<int>(roundabout_type)], modifier};
  }

  static TurnInstruction
  ENTER_ROUNDABOUT_AT_EXIT(const RoundaboutType roundabout_type,
                           const DirectionModifier modifier) {
    const constexpr TurnType enter_instruction[] = {
        TurnType::Invalid, TurnType::EnterRoundaboutAtExit,
        TurnType::EnterRotaryAtExit,
        TurnType::EnterRoundaboutIntersectionAtExit};
    return {enter_instruction[static_cast<int>(roundabout_type)], modifier};
  }

  static TurnInstruction SUPPRESSED(const DirectionModifier modifier) {
    return {TurnType::Suppressed, modifier};
  }
};

static_assert(sizeof(TurnInstruction) == 8,
              "TurnInstruction does not fit 8 bytes");

inline bool operator!=(const TurnInstruction lhs, const TurnInstruction rhs) {
  return lhs.type != rhs.type ||
         lhs.direction_modifier != rhs.direction_modifier;
}

inline bool operator==(const TurnInstruction lhs, const TurnInstruction rhs) {
  return lhs.type == rhs.type &&
         lhs.direction_modifier == rhs.direction_modifier;
}

inline bool has_roundabout_type(const TurnInstruction instruction) {
  const constexpr TurnType valid_types[] = {
      TurnType::EnterRoundabout,
      TurnType::EnterAndExitRoundabout,
      TurnType::EnterRotary,
      TurnType::EnterAndExitRotary,
      TurnType::EnterRoundaboutIntersection,
      TurnType::EnterAndExitRoundaboutIntersection,
      TurnType::EnterRoundaboutAtExit,
      TurnType::ExitRoundabout,
      TurnType::EnterRotaryAtExit,
      TurnType::ExitRotary,
      TurnType::EnterRoundaboutIntersectionAtExit,
      TurnType::ExitRoundaboutIntersection,
      TurnType::StayOnRoundabout};

  const auto *first = valid_types;
  const auto *last = first + sizeof(valid_types) / sizeof(valid_types[0]);

  return std::find(first, last, instruction.type) != last;
}

inline bool enters_roundabout(const guidance::TurnInstruction instruction) {
  return (instruction.type == guidance::TurnType::EnterRoundabout ||
          instruction.type == guidance::TurnType::EnterRotary ||
          instruction.type == guidance::TurnType::EnterRoundaboutIntersection ||
          instruction.type == guidance::TurnType::EnterRoundaboutAtExit ||
          instruction.type == guidance::TurnType::EnterRotaryAtExit ||
          instruction.type ==
              guidance::TurnType::EnterRoundaboutIntersectionAtExit ||
          instruction.type == guidance::TurnType::EnterAndExitRoundabout ||
          instruction.type == guidance::TurnType::EnterAndExitRotary ||
          instruction.type ==
              guidance::TurnType::EnterAndExitRoundaboutIntersection);
}

inline bool leaves_roundabout(const guidance::TurnInstruction instruction) {
  return (instruction.type == guidance::TurnType::ExitRoundabout ||
          instruction.type == guidance::TurnType::ExitRotary ||
          instruction.type == guidance::TurnType::ExitRoundaboutIntersection ||
          instruction.type == guidance::TurnType::EnterAndExitRoundabout ||
          instruction.type == guidance::TurnType::EnterAndExitRotary ||
          instruction.type ==
              guidance::TurnType::EnterAndExitRoundaboutIntersection);
}

inline bool stays_on_roundabout(const guidance::TurnInstruction instruction) {
  return instruction.type == guidance::TurnType::StayOnRoundabout ||
         instruction.type == guidance::TurnType::EnterRoundaboutAtExit ||
         instruction.type == guidance::TurnType::EnterRotaryAtExit ||
         instruction.type ==
             guidance::TurnType::EnterRoundaboutIntersectionAtExit;
}

inline bool is_silent(const guidance::TurnInstruction instruction) {
  return instruction.type == guidance::TurnType::NoTurn ||
         instruction.type == guidance::TurnType::Suppressed ||
         instruction.type == guidance::TurnType::StayOnRoundabout;
}

inline bool has_ramp_type(const guidance::TurnInstruction instruction) {
  return instruction.type == guidance::TurnType::OffRamp ||
         instruction.type == guidance::TurnType::OnRamp;
}

inline guidance::DirectionModifier get_turn_direction(const double angle) {
  if (angle > 0 && angle < 60)
    return guidance::DirectionModifier::SharpRight;
  if (angle >= 60 && angle < 140)
    return guidance::DirectionModifier::Right;
  if (angle >= 140 && angle < 160)
    return guidance::DirectionModifier::SlightRight;
  if (angle >= 160 && angle <= 200)
    return guidance::DirectionModifier::Straight;
  if (angle > 200 && angle <= 220)
    return guidance::DirectionModifier::SlightLeft;
  if (angle > 220 && angle <= 300)
    return guidance::DirectionModifier::Left;
  if (angle > 300 && angle < 360)
    return guidance::DirectionModifier::SharpLeft;
  return guidance::DirectionModifier::UTurn;
}

inline guidance::DirectionModifier
mirror_direction_modifier(const guidance::DirectionModifier modifier) {
  const constexpr guidance::DirectionModifier results[] = {
      guidance::DirectionModifier::UTurn,
      guidance::DirectionModifier::SharpLeft,
      guidance::DirectionModifier::Left,
      guidance::DirectionModifier::SlightLeft,
      guidance::DirectionModifier::Straight,
      guidance::DirectionModifier::SlightRight,
      guidance::DirectionModifier::Right,
      guidance::DirectionModifier::SharpRight};
  return results[modifier];
}

inline bool has_left_modifier(const guidance::TurnInstruction instruction) {
  return instruction.direction_modifier ==
             guidance::DirectionModifier::SharpLeft ||
         instruction.direction_modifier == guidance::DirectionModifier::Left ||
         instruction.direction_modifier ==
             guidance::DirectionModifier::SlightLeft;
}

inline bool has_right_modifier(const guidance::TurnInstruction instruction) {
  return instruction.direction_modifier ==
             guidance::DirectionModifier::SharpRight ||
         instruction.direction_modifier == guidance::DirectionModifier::Right ||
         instruction.direction_modifier ==
             guidance::DirectionModifier::SlightRight;
}

inline bool is_left_turn(const guidance::TurnInstruction instruction) {
  switch (instruction.type) {
  case TurnType::Merge:
    return has_right_modifier(instruction);
  default:
    return has_left_modifier(instruction);
  }
}

inline bool is_right_turn(const guidance::TurnInstruction instruction) {
  switch (instruction.type) {
  case TurnType::Merge:
    return has_left_modifier(instruction);
  default:
    return has_right_modifier(instruction);
  }
}

inline DirectionModifier bearing_to_direction_modifier(const double bearing) {
  if (bearing < 135) {
    return guidance::DirectionModifier::Right;
  }

  if (bearing <= 225) {
    return guidance::DirectionModifier::Straight;
  }
  return guidance::DirectionModifier::Left;
}

namespace detail {

const constexpr char *modifier_names[] = {
    "uturn",       "sharp right", "right",      "slight right", "straight",
    "slight left", "left",        "sharp left", "UNDEFINED"};

struct TurnTypeName {
  const char *external_name, *internal_name;
};

const constexpr TurnTypeName turn_type_names[] = {
    {"invalid", "(not set)"},
    {"new name", "new name"},
    {"continue", "continue"},
    {"turn", "turn"},
    {"merge", "merge"},
    {"on ramp", "on ramp"},
    {"off ramp", "off ramp"},
    {"fork", "fork"},
    {"end of road", "end of road"},
    {"notification", "notification"},
    {"roundabout", "enter roundabout"},
    {"exit roundabout", "enter and exit roundabout"},
    {"rotary", "enter rotary"},
    {"exit rotary", "enter and exit rotary"},
    {"roundabout turn", "enter roundabout turn"},
    {"roundabout turn", "enter and exit roundabout turn"},
    {"use lane", "use lane"},
    {"invalid", "(noturn)"},
    {"invalid", "(suppressed)"},
    {"roundabout", "roundabout"},
    {"exit roundabout", "exit roundabout"},
    {"rotary", "rotary"},
    {"exit rotary", "exit rotary"},
    {"roundabout turn", "roundabout turn"},
    {"exit roundabout", "exit roundabout turn"},
    {"invalid", "(stay on roundabout)"},
    {"invalid", "(sliproad)"},
    {"MAXVALUE", "MAXVALUE"}};

} // namespace detail

inline std::string instruction_type_to_string(const TurnType type) {
  static_assert((sizeof(detail::turn_type_names) + 1) /
                        sizeof(detail::turn_type_names[0]) >=
                    TurnType::MaxTurnType,
                "Some turn types have no string representation.");
  return detail::turn_type_names[static_cast<std::size_t>(type)].external_name;
}

inline std::string internal_instruction_type_to_string(const TurnType type) {
  static_assert((sizeof(detail::turn_type_names) + 1) /
                        sizeof(detail::turn_type_names[0]) >=
                    TurnType::MaxTurnType,
                "Some turn types have no string representation.");
  return detail::turn_type_names[static_cast<std::size_t>(type)].internal_name;
}

inline std::string
instruction_modifier_to_string(const DirectionModifier modifier) {
  static_assert((sizeof(detail::modifier_names) + 1) /
                        sizeof(detail::modifier_names[0]) >=
                    DirectionModifier::MaxDirectionModifier,
                "Some direction modifiers have no string representation.");
  return detail::modifier_names[static_cast<std::size_t>(modifier)];
}
} // namespace guidance
} // namespace osram
#endif
