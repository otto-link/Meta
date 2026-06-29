import pandas as pd
import sys


def df_to_markdown(df):
    """Convert a pandas DataFrame to a Markdown table."""
    df = df.fillna("")

    header = "| " + " | ".join(map(str, df.columns)) + " |"
    separator = "| " + " | ".join(["---"] * len(df.columns)) + " |"

    rows = []
    for _, row in df.iterrows():
        rows.append("| " + " | ".join(map(str, row.values)) + " |")

    return "\n".join([header, separator] + rows)


def ods_to_markdown(file_path, sheet_name=None):
    # Read ODS file
    xls = pd.read_excel(file_path, engine="odf", sheet_name=None)

    markdown_output = []

    sheets = [sheet_name] if sheet_name else xls.keys()

    for name in sheets:
        df = xls[name]
        md = df_to_markdown(df)

        markdown_output.append(f"## Sheet: {name}\n\n{md}")

    return "\n\n".join(markdown_output)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python ods_to_md.py file.ods [sheet_name]")
        sys.exit(1)

    file_path = sys.argv[1]
    sheet_name = sys.argv[2] if len(sys.argv) > 2 else None

    result = ods_to_markdown(file_path, sheet_name)
    print(result)
