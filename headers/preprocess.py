import pandas as pd
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
import re
import sys
import string
import numpy as np
from nltk.stem import WordNetLemmatizer

nltk.download('wordnet')
nltk.download('omw-1.4')
nltk.download('stopwords')
nltk.download('punkt')
stopwords = set(stopwords.words('english'))

f = open(sys.argv[1],"r+")
texto = f.read()

def clean(text):
  text = str(text).lower()
  text = re.sub(r"\[(.*?)\]", "", text) 
  text = re.sub(r"\s+", " ", text) 
  text = re.sub(r"\w+…|…", "", text) 
  text = re.sub(r"(?<=\w)-(?=\w)", " ", text)
  text = re.sub(
      f"[{re.escape(string.punctuation)}]", "", text
  )  # Remove punctuation

  lemmatizer = WordNetLemmatizer()
  tokens = nltk.word_tokenize(text)  # Get tokens from text
  tokens = [t for t in tokens if not t in stopwords]  # Remove stopwords
  tokens = ["" if t.isdigit() else t for t in tokens]  # Remove digits
  tokens = [t for t in tokens if len(t) > 1]  # Remove short tokens
  tokens = [lemmatizer.lemmatize(t) for t in tokens] # lemmatize

  return tokens

df = pd.DataFrame({"text" : clean(texto)})
df["text"] = df["text"].fillna("")
df["text"] = df["text"].astype(str)

# df = df.loc[df.t_text.map(lambda x: len(x) > 0), ["text"]]

docs = df["text"].values

#df['new_text'] = df['text'].apply(lambda x: ' '.join(x))
out_file = ' '.join(docs)

out = open(sys.argv[2],"w+")
out.write(out_file)
out.close()
