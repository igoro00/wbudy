import xml.etree.ElementTree as ET
import sys

# ANSI color codes
COLORS = {
    "information": "\033[94m",  # Blue
    "warning": "\033[93m",        # Yellow
    "style": "\033[95m",      # Magenta
    "portability": "\033[96m",   # Cyan
    "error": "\033[91m",        # Red
    "reset": "\033[0m"
}

SEVERITIES = ["information", "style", "portability", "warning", "error"]  # Ordered by importance


def parse_cppcheck_results(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    errors_by_severity = {sev: [] for sev in SEVERITIES}

    error_count = 0
    for error in root.iter("error"):
        error_id = error.get("id")
        severity = error.get("severity")
        msg = error.get("msg")

        if severity not in SEVERITIES:
            print(f"Unknown severity encountered: {severity}")
            sys.exit(2)

        location = error.find("location")
        if location is None:
            continue

        file = location.get("file")
        line = location.get("line")
        column = location.get("column")

        errors_by_severity[severity].append(
            (file, line, column, severity, msg, error_id)
        )
        error_count += 1

    for severity in SEVERITIES:
        color = COLORS[severity]
        reset = COLORS["reset"]
        for file, line, column, sev, msg, error_id in errors_by_severity[severity]:
            print(
                f"{file}:{line}:{column}: {color}{sev}{reset}: {msg} [{error_id}]\n"
            )
    print(f"\nTotal errors found: {error_count}")

    if error_count>0:
        sys.exit(1)


if len(sys.argv) != 2:
    print("Usage: python parse_xml.py <xml_file>")
    sys.exit(1)

xml_file = sys.argv[1]
parse_cppcheck_results(xml_file)
