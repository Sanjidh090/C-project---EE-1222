# text_to_pdf.py
from fpdf import FPDF
import sys

def text_to_pdf(input_file, output_file):
    pdf = FPDF()
    pdf.add_page()
    pdf.set_font("Arial", size=16)
    
    with open(input_file, 'r') as file:
        for line in file:
            pdf.cell(200, 10, txt=line, ln=True, align='L')
    
    pdf.output(output_file)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python text_to_pdf.py input.txt output.pdf")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    text_to_pdf(input_file, output_file)
