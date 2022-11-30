import pandas as pd
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from gensim.models import Word2Vec
import re
import sys
import string
import numpy as np

nltk.download('stopwords')
nltk.download('punkt')
stopwords = set(stopwords.words('english'))

print("\nName of Python script:", sys.argv[0])

def clean(text):
  text = str(text).lower()
  text = re.sub(r"\[(.*?)\]", "", text) 
  text = re.sub(r"\s+", " ", text) 
  text = re.sub(r"\w+…|…", "", text) 
  text = re.sub(r"(?<=\w)-(?=\w)", " ", text)
  text = re.sub(
      f"[{re.escape(string.punctuation)}]", "", text
  )  # Remove punctuation

  tokens = nltk.word_tokenize(text)  # Get tokens from text
  tokens = [t for t in tokens if not t in stopwords]  # Remove stopwords
  tokens = ["" if t.isdigit() else t for t in tokens]  # Remove digits
  tokens = [t for t in tokens if len(t) > 1]  # Remove short tokens

  return tokens

df = dataset.copy()
df["text"] = df["text"].fillna("")
df["text"] = df["text"].astype(str)
df["t_text"] = df["text"].map(lambda x: clean(x))

df = df.loc[df.t_text.map(lambda x: len(x) > 0), ["text", "t_text"]]

docs = df["text"].values
tokenized_docs = df["t_text"].values

df['new_text'] = df['t_text'].apply(lambda x: ' '.join(x))
out_file = ' '.join(df['new_text'])

f = open("output.txt","w+")
f.write(out_file)
f.close()
