from pathlib import Path

from reportlab.lib import colors
from reportlab.lib.enums import TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.lib.utils import ImageReader
from reportlab.platypus import (
    Image,
    PageBreak,
    Paragraph,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / "output" / "pdf" / "CppShell_Mid_Progress_Review_24BCI0232_Checked.pdf"
OUTPUT.parent.mkdir(parents=True, exist_ok=True)

styles = getSampleStyleSheet()
styles.add(
    ParagraphStyle(
        name="ReviewBody",
        parent=styles["BodyText"],
        fontName="Times-Roman",
        fontSize=10.8,
        leading=14.0,
        spaceAfter=5.5,
        textColor=colors.black,
    )
)
styles.add(
    ParagraphStyle(
        name="ReviewCaption",
        parent=styles["BodyText"],
        fontName="Helvetica",
        fontSize=8.8,
        leading=11.2,
        spaceBefore=4,
        spaceAfter=7,
        alignment=TA_LEFT,
        textColor=colors.HexColor("#333333"),
    )
)
styles.add(
    ParagraphStyle(
        name="ReviewSmall",
        parent=styles["BodyText"],
        fontName="Times-Roman",
        fontSize=9.7,
        leading=12.2,
        spaceAfter=2,
        textColor=colors.black,
    )
)
styles.add(
    ParagraphStyle(
        name="ReviewHeading",
        parent=styles["Heading2"],
        fontName="Helvetica-Bold",
        fontSize=12.6,
        leading=15.5,
        spaceBefore=8,
        spaceAfter=6,
        keepWithNext=True,
        textColor=colors.black,
    )
)
styles.add(
    ParagraphStyle(
        name="ReviewTitle",
        parent=styles["Title"],
        fontName="Helvetica-Bold",
        fontSize=18.5,
        leading=22,
        alignment=TA_LEFT,
        spaceBefore=2,
        spaceAfter=12,
        textColor=colors.black,
    )
)
styles.add(
    ParagraphStyle(
        name="ReviewBullet",
        parent=styles["BodyText"],
        fontName="Times-Roman",
        fontSize=10.7,
        leading=13.8,
        leftIndent=13,
        firstLineIndent=-8,
        bulletIndent=1,
        spaceAfter=4.5,
        textColor=colors.black,
    )
)

story = []


def para(text, style="ReviewBody"):
    story.append(Paragraph(text, styles[style]))


def heading(text):
    para(text, "ReviewHeading")


def bullet(text):
    story.append(Paragraph(text, styles["ReviewBullet"], bulletText="-"))


def add_table(headers, rows, widths):
    data = [headers, *rows]
    formatted = [
        [Paragraph(str(value), styles["ReviewSmall"]) for value in row]
        for row in data
    ]
    table = Table(formatted, colWidths=widths, repeatRows=1, hAlign="LEFT")
    table.setStyle(
        TableStyle(
            [
                ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#E8EDF2")),
                ("TEXTCOLOR", (0, 0), (-1, 0), colors.black),
                ("FONTNAME", (0, 0), (-1, 0), "Helvetica-Bold"),
                ("GRID", (0, 0), (-1, -1), 0.45, colors.HexColor("#D2D7DC")),
                ("VALIGN", (0, 0), (-1, -1), "MIDDLE"),
                ("LEFTPADDING", (0, 0), (-1, -1), 7),
                ("RIGHTPADDING", (0, 0), (-1, -1), 7),
            ("TOPPADDING", (0, 0), (-1, -1), 4),
            ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
            ]
        )
    )
    story.append(table)
    story.append(Spacer(1, 5))


def scaled_image(path, width_mm):
    source_width, source_height = ImageReader(str(path)).getSize()
    width = width_mm * mm
    height = width * source_height / source_width
    return Image(str(path), width=width, height=height)


def add_figure(path, width_mm, caption):
    image = scaled_image(path, width_mm)
    image.hAlign = "CENTER"
    story.append(image)
    para(caption, "ReviewCaption")


SCREENSHOT_DIR = ROOT / "tmp" / "pdfs" / "user-edited-review" / "extracted"
SCREENSHOTS = {
    "build": SCREENSHOT_DIR / "figure1.png",
    "builtins": SCREENSHOT_DIR / "figure2.png",
    "filesystem_a": SCREENSHOT_DIR / "figure3.png",
    "filesystem_b": SCREENSHOT_DIR / "figure4.png",
    "navigation_errors": SCREENSHOT_DIR / "figure5.png",
    "navigation_paths": SCREENSHOT_DIR / "figure6.png",
}


para(
    'GitHub repository: <link href="https://github.com/std7777/shell" '
    'color="#174A77">https://github.com/std7777/shell</link>'
)
para("CppShell Mid Progress Review", "ReviewTitle")

heading("1 Project Title and Team Details")
para(
    "<b>Project title:</b> Design and Implementation of a POSIX-Compatible Shell Using C++<br/>"
    "<b>Student:</b> Vachhani Priyanshi Hetalkumar<br/>"
    "<b>Registration number:</b> 24BCI0232<br/>"
    "<b>Course:</b> Operating Systems<br/>"
    "<b>Guide or faculty:</b> SUCHITHRA J<br/>"
    "<b>Department:</b> CSE with Information Security<br/>"
    "<b>Review date:</b> 7 October 2026"
)

heading("2 Introduction and Background")
para(
    "A command-line shell accepts textual commands and connects them to operating-system "
    "services. CppShell is a C++23 project that exposes this flow through a compact interactive "
    "implementation. The current version supports command dispatch, built-in commands, executable "
    "discovery through PATH, external command execution, and persistent directory navigation."
)
para(
    "The project demonstrates environment-variable handling, executable permissions, filesystem "
    "state, standard input and output, and the distinction between operations performed inside the "
    "shell process and programs launched outside it. Planned stages extend these ideas to child "
    "processes, file descriptors, redirection, and inter-process communication through pipes."
)

heading("3 Problem Statement")
para(
    "The project addresses how a shell interprets user input, distinguishes built-in commands from "
    "external programs, locates executables, maintains process state such as the current working "
    "directory, and reports errors without ending the interactive session. The final scope also "
    "includes direct process creation, stream redirection, and pipelines."
)
para(
    "The target is a defined subset of POSIX shell behavior. Shell scripting, job control, "
    "background execution, wildcard expansion, command substitution, aliases, functions, and "
    "persistent command history remain outside the initial scope."
)

heading("4 Objectives")
bullet("Provide a persistent prompt that reads and dispatches commands until the user exits.")
bullet("Implement echo, exit, type, pwd, and cd, including absolute, relative, and home-directory navigation.")
bullet("Locate external executables through PATH and run them with command-line arguments.")
bullet("Add direct process execution, output and error redirection, and multi-command pipelines.")
bullet("Manage child processes and file descriptors safely and recover from command errors.")

heading("5 Methodology and System Design")
para(
    "Development proceeds feature by feature. The original single-file program has been divided "
    "into an entry point, shared lookup utilities, a command dispatcher, and one handler for each "
    "built-in. The dispatcher maps command names to functions, allowing built-ins to run in the "
    "CppShell process while external commands follow a separate lookup and execution path."
)
para(
    "<b>Current execution flow:</b> main.cpp reads PATH and stores its valid directories. The REPL "
    "prints a prompt and reads a line. The dispatcher separates the command name from the remaining "
    "input. Registered built-ins execute directly. Other names are searched in PATH and, if found, "
    "the complete input line is passed to std::system()."
)

add_table(
    ["Current component", "Responsibility"],
    [
        ("main.cpp", "Initializes stream flushing, reads PATH, prepares executable lookup, and starts the REPL."),
        ("command_handler.hpp", "Registers five built-ins, splits input, dispatches commands, and controls the prompt loop."),
        ("common/common.hpp", "Stores built-in names and PATH directories and validates executable candidates."),
        ("builtins/echo.hpp", "Prints the input text."),
        ("builtins/exit.hpp", "Returns the control value that ends the REPL."),
        ("builtins/type.hpp", "Identifies built-ins or prints the first executable path found through PATH."),
        ("builtins/pwd.hpp", "Reads and prints the shell process's current working directory."),
        ("builtins/cd.hpp", "Changes the shell process's working directory and expands a leading tilde through HOME."),
    ],
    [53 * mm, 127 * mm],
)

para(
    "<b>Operating-system behavior:</b> pwd and cd run as built-ins inside CppShell. This is necessary "
    "for cd because changing the directory of a separate child process would not change the shell's "
    "own working directory. The implemented cd uses std::filesystem::current_path(), so successful "
    "changes remain active for later commands."
)
para(
    "<b>Current execution boundary:</b> std::system() delegates external execution and waiting to the "
    "host command interpreter. CppShell does not yet create child processes with fork(), replace a "
    "child with exec(), or collect status with waitpid(). It also does not yet create or connect file "
    "descriptors for redirection and pipelines."
)
para(
    "<b>Technologies and concepts:</b> C++23, CMake, Git, STL containers and function objects, "
    "std::filesystem, PATH and HOME environment variables, executable-permission checks through "
    "access(..., X_OK), standard streams, and persistent process state."
)

heading("6 Work Completed and Current Progress")
add_table(
    ["Feature", "Current state"],
    [
        ("Interactive prompt and dispatch", "Implemented for ordinary command input, built-in lookup, external lookup, and unavailable-command diagnostics."),
        ("echo, exit, and type", "Implemented as separate handlers. type recognizes all five current built-ins and resolves external executables through PATH."),
        ("PATH executable resolution", "Implemented in directory order with existence, regular-file, and executable-permission checks."),
        ("External execution", "Implemented through std::system() after PATH lookup. Direct fork/exec/wait handling remains pending."),
        ("pwd", "Implemented using std::filesystem::current_path()."),
        ("cd", "Implemented for filesystem paths; relative and absolute paths are handled by current_path(), and a leading tilde is expanded from HOME."),
        ("Parser, redirection, pipelines", "A first-space command split exists. Structured parsing, native redirection, and pipeline management remain pending."),
    ],
    [59 * mm, 121 * mm],
)

para(
    "<b>Current status:</b> the project now completes the planned basic built-in set: echo, exit, "
    "type, pwd, and cd. PATH-based executable discovery and delegated external execution are also "
    "present. Directory changes occur within the long-running shell process and therefore affect "
    "subsequent pwd and external commands."
)
para(
    "<b>Current limitations:</b> input is still split only at the first space. Empty input and "
    "end-of-file are not handled explicitly. cd accesses its first input character before checking "
    "whether the input is empty, and its error message does not distinguish missing paths from "
    "permission or other filesystem errors. A nonzero result returned by std::system() is currently "
    "treated as a signal to end the REPL."
)

heading("Working Model Demonstration")
para(
    "The terminal screenshots below record the working model presented on 6 October 2026. The Kali "
    "prompt is visible before CppShell starts. After <font name='Courier'>./build/shell</font> is "
    "executed, CppShell uses its own simple <font name='Courier'>$</font> prompt."
)
add_figure(
    SCREENSHOTS["build"],
    145,
    "Figure 1  Configuring the project with CMake, compiling the target, creating the shell executable, launching it from the terminal, and confirming cd, pwd, exit, and echo behavior.",
)

story.append(PageBreak())

heading("Built-in and External Command Demonstration")
para(
    "This run shows built-in output, an external program found through PATH, working-directory "
    "changes that persist inside CppShell, and diagnostics for unavailable commands."
)
add_figure(
    SCREENSHOTS["builtins"],
    145,
    "Figure 2  Running echo, the external cat program, cd, pwd, home-directory navigation, exit, and unavailable-command handling.",
)

story.append(PageBreak())

heading("Executable Resolution and Filesystem Commands")
para(
    "The two sessions distinguish host-shell output from commands entered after CppShell starts. "
    "They demonstrate type resolution and external filesystem utilities executed through CppShell."
)
left_figure = Table(
    [[scaled_image(SCREENSHOTS["filesystem_a"], 78)],
     [Paragraph("Figure 3  type identifies cd as a built-in and resolves cat through PATH; rm and ls run as external programs.", styles["ReviewCaption"])]],
    colWidths=[82 * mm],
)
right_figure = Table(
    [[scaled_image(SCREENSHOTS["filesystem_b"], 82)],
     [Paragraph("Figure 4  touch, ls, rm, mkdir, and rm -rf demonstrate external programs with arguments and visible filesystem effects.", styles["ReviewCaption"])]],
    colWidths=[86 * mm],
)
for figure_column in (left_figure, right_figure):
    figure_column.setStyle(TableStyle([
        ("VALIGN", (0, 0), (-1, -1), "TOP"),
        ("LEFTPADDING", (0, 0), (-1, -1), 0),
        ("RIGHTPADDING", (0, 0), (-1, -1), 0),
        ("TOPPADDING", (0, 0), (-1, -1), 0),
        ("BOTTOMPADDING", (0, 0), (-1, -1), 2),
    ]))
paired = Table(
    [[left_figure, right_figure]],
    colWidths=[86 * mm, 86 * mm],
    hAlign="CENTER",
)
paired.setStyle(TableStyle([
    ("VALIGN", (0, 0), (-1, -1), "TOP"),
    ("LEFTPADDING", (0, 0), (-1, -1), 2),
    ("RIGHTPADDING", (0, 0), (-1, -1), 2),
    ("TOPPADDING", (0, 0), (-1, -1), 2),
    ("BOTTOMPADDING", (0, 0), (-1, -1), 2),
]))
story.append(paired)

story.append(PageBreak())

heading("Directory Navigation and Error Cases")
para(
    "These sessions show successful relative and absolute directory changes, home-directory "
    "expansion, persistent pwd results, type recognition, and current cd error messages."
)
add_figure(
    SCREENSHOTS["navigation_errors"],
    130,
    "Figure 5  Rebuilding and launching CppShell, then exercising invalid paths, relative navigation, parent navigation, type cd, root navigation, and a missing cd argument.",
)
add_figure(
    SCREENSHOTS["navigation_paths"],
    150,
    "Figure 6  Navigating by absolute path, root path, and home-relative path, followed by pwd and external touch and ls commands.",
)

story.append(PageBreak())

heading("7 Last Contributions and Recent Progress")
para(
    "The latest development phase added filesystem navigation and connected the new commands to the "
    "existing built-in registry. The project history records these milestones:"
)
add_table(
    ["Recorded period", "Development milestone"],
    [
        ("20 February 2026", "Added the prompt, user input, and unavailable-command handling."),
        ("27 to 28 February 2026", "Added the repeated prompt, exit, echo, and built-in identification through type."),
        ("7 March 2026", "Extended type to search executable programs through PATH."),
        ("25 to 26 August 2026", "Separated the program into modules and connected PATH lookup to external execution."),
        ("5 October 2026", "Added pwd and cd, registered both built-ins, and added leading-tilde expansion through HOME."),
    ],
    [45 * mm, 135 * mm],
)
para(
    "The main new result since the previous review is persistent directory navigation. pwd exposes "
    "the current process state, while cd changes that state for later commands. Home-directory "
    "expansion also connects shell syntax to the HOME environment variable."
)

story.append(PageBreak())

heading("8 Remaining Work and Validation")
para("The following features and corrections remain within the defined project scope:")
bullet("<b>Reliable input handling:</b> handle blank input, leading whitespace, missing cd arguments, and end-of-file safely.")
bullet("<b>Filesystem error reporting:</b> preserve the reason reported by the operating system so cd can distinguish a missing path, permission failure, and other errors.")
bullet("<b>Structured command parsing:</b> tokenize command names, arguments, redirection operators, filenames, and pipeline separators before allocating operating-system resources.")
bullet("<b>Direct external execution:</b> replace std::system() with fork(), an exec-family operation, and waitpid(), and pass an explicit argument vector to the child program.")
bullet("<b>Exit-status handling:</b> separate the exit built-in from external-program status so a failed child command does not terminate the shell.")
bullet("<b>Output and error redirection:</b> implement overwrite and append modes for standard output and standard error using open(), dup2(), and close().")
bullet("<b>Built-in redirection:</b> redirect supported built-ins while restoring the parent shell's standard streams after execution.")
bullet("<b>Pipelines:</b> implement two-command and multi-command pipelines with pipe(), including supported built-ins as pipeline stages.")
bullet("<b>Resource management:</b> close unused descriptors, wait for every child, and handle partial failures without leaks, unreaped processes, or deadlocks.")
bullet("<b>Prompt and environment improvements:</b> show useful location information in the prompt and make environment-dependent behavior update safely during the session.")

para(
    "<b>Validation still required:</b> check PATH ordering and executable permissions; empty input "
    "and end-of-file; valid and invalid cd targets; relative, absolute, and home paths; failed "
    "external programs; overwrite and append modes; stream restoration; multi-stage pipelines; "
    "large pipeline input; child reaping; and descriptor cleanup."
)
para(
    "<b>Documentation:</b> add build and usage instructions, supported command examples, known "
    "limitations, and repeatable checks for each implemented feature."
)
para(
    "<b>Expected final outcome:</b> a modular C++ shell that executes supported built-ins and "
    "external programs, preserves working-directory state, redirects standard output and error, and "
    "coordinates multi-command pipelines. The result will demonstrate process creation, program "
    "replacement, synchronization, file-descriptor management, filesystem state, and inter-process "
    "communication."
)


def footer(canvas, document):
    canvas.saveState()
    canvas.setFont("Helvetica", 8.5)
    canvas.setFillColor(colors.HexColor("#555555"))
    canvas.drawString(18 * mm, 10 * mm, "CppShell | Operating Systems | Mid Progress Review")
    canvas.drawRightString(A4[0] - 18 * mm, 10 * mm, str(document.page))
    canvas.restoreState()


document = SimpleDocTemplate(
    str(OUTPUT),
    pagesize=A4,
    leftMargin=18 * mm,
    rightMargin=18 * mm,
    topMargin=14 * mm,
    bottomMargin=17 * mm,
    title="CppShell Mid Progress Review",
    author="Vachhani Priyanshi Hetalkumar",
)
document.build(story, onFirstPage=footer, onLaterPages=footer)
print(OUTPUT)
