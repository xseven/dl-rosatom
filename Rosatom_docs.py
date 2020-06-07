#!/usr/bin/env python
# coding: utf-8

# ## Ввод датасета/нашей документации

# In[1]:


from striprtf.striprtf import rtf_to_text
import os
from nltk.tokenize import LineTokenizer
import pandas as pd
#import json


# In[2]:


myPath="./docs/"


# In[3]:


ltokenizer=LineTokenizer()


# In[4]:


def get_rtf_names(loc_path):
    rtf_names=[]
    names=os.listdir(path=loc_path)
    for name in names:
        if ".rtf" in name:
            rtf_names.append(loc_path+name)
    return rtf_names


# In[ ]:





# In[5]:


def get_text_doc(fname):
    file=open(fname, 'r')
    rtfdoc=file.read()
    document=rtf_to_text(rtfdoc)
    return document


# In[6]:


def parse_hd(document, tokenizer=ltokenizer):
    lines=tokenizer.tokenize(document)
    tok_head="Должностная инструкция"
    i_head=0
    cnt=0
    for l in lines:
        if tok_head in l:
            i_head=cnt
            break
        cnt+=1
    hline=lines[i_head]
    if i_head<len(lines)-1:
        hline+=lines[i_head+1]
    return hline, lines


# In[7]:


def get_data_text_pd(paragraph_breaker=ltokenizer):
    rtf_names=get_rtf_names(myPath)
    #rtf_names=["./docs/005.rtf", "./docs/006.rtf"]
    df=pd.DataFrame(index=range(0,len(rtf_names)), columns=["Position", "Text","File name"])
    cnt=0
    for name in rtf_names:
        document=get_text_doc(name)
        hline, lines=parse_hd(document,tokenizer=paragraph_breaker)
        df.iloc[cnt, 0]=hline
        df.iloc[cnt, 1]=lines
        df.iloc[cnt, 2]=name
        cnt+=1
    return df


# In[8]:


all_data=get_data_text_pd()


# In[9]:


def dump_json(data,local_path="./docs/"):
    short_data=pd.DataFrame(index=all_data.index, columns=["id", "Position", "path"])
    short_data["id"]=all_data.index.copy(deep=True)
    short_data["Position"]=all_data.Position.copy(deep=True)
    short_data["path"]=all_data["File name"].copy(deep=True)
    short_data.to_json(path_or_buf=local_path+"gen_table.json", orient='records')


# In[10]:


dump_json(all_data)


# In[11]:


def prepare_text(init_text, tokenizer, stop_words, morpher):
    init_text=init_text.lower()
    tokens=tokenizer.tokenize(init_text)
    tokens_clear=[]
    for t in tokens:
        if t in stop_words:
            continue
        tokens_clear.append(t)
    tokens_lemmatized=[]
    for t in tokens_clear:
        p = morph.parse(t)[0]
        p=p.normal_form
        tokens_lemmatized.append(p)
    return tokens_lemmatized


# ## Готовим инструменты

# In[12]:


import re
import logging

import gensim.downloader as api
import nltk
from nltk.corpus import stopwords
#from nltk.stem.wordnet import WordNetLemmatizer
from nltk.tokenize import RegexpTokenizer

import pymorphy2

#from nltk import pos_tag_sents
#from nltk.tokenize import word_tokenize
#nltk.download('punkt')
#nltk_download('averaged_perceptron_tagger_ru')
#nltk.download('universal_tagset')
#pos_tag_sents(word_tokenize("настоящей должностной инструкцией"), tagset='universal',lang='rus')


# ## Подключаем gensim и загружаем векторную модель слов (уже готовую, model.model)

# In[13]:


import gensim


# In[14]:


logging.basicConfig(format="%(levelname)s - %(asctime)s: %(message)s", datefmt= '%H:%M:%S', level=logging.INFO)


# In[15]:


api.BASE_DIR='.\\docs\\model\\' #myPath+'model/' #C:\\Users\\Home\\gensim-data\\'


# In[16]:


api.BASE_DIR


# In[17]:


#model=api.load("word2vec-ruscorpora-300")


# In[18]:


mod_path=os.getcwd()


# In[19]:


mod_path=os.getcwd()+'\\docs\\model\\model.model'


# In[20]:


mod_path


# In[21]:


md=gensim.models.KeyedVectors.load(mod_path) #("C:\\Users\\Home\\gensim-data\\model.model") #C:\\Users\\Home\\gensim-data\\model.model


# ## Получаем входной текст и предобрабатываем его: леммтизация, токенизация, выкидывание стоп-слов, частых/редких слов

# In[22]:


import sys


# In[23]:


if len(sys.argv)>3:
    hdr=sys.argv[1]
    init_text=hdr+sys.argv[2]+sys.argv[3]
else:
    hdr="Электрик"
    init_text="службы начальника производства испытания устройств и электротехнических измерений монтаж сетей электрообородования"


# In[24]:


#hdr="Генеральный директор"
#init_text="Администрация, руководство персоналом, работа с документами"


# In[25]:


stop_words = stopwords.words('russian')


# ### Разбиваем текст на отдельные токены (здесь, по сути, на слова)

# In[26]:


tokenizer = RegexpTokenizer('\w+|[^\w\s]+\.') # с паттернами сложно, надо отрегулировать #'\w+|[^\w\s]+'


# In[27]:


morph = pymorphy2.MorphAnalyzer()


# In[28]:


tokens_lemmatized=prepare_text(init_text, tokenizer, stop_words, morph)


# ## Получаем входную базу текста

# In[29]:


#headers, data_text, the_rest=get_data_text()
# pd.DataFrame - all_data


# ## Получаем рекомендуемый текст и заголовки

# ### Сначала - топ рекомендуемых документов, выбранных по заголовкам

# In[30]:


def rem_token(lst,token):
    n=lst.count(token)
    if n>0:
        lst.remove(token)
        rem_token(lst,token)
    else:
        return


# In[31]:


def get_top_docs(hdr, data, REtokenizer, stop_words, morpher, model, n=5):
    hdr_lemmatized=prepare_text(hdr, REtokenizer, stop_words, morpher)
    cnt=0
    header_sim=[]
    token1="должностной"
    token2="инструкция"
    for i in range(0,data.shape[0]):
        position=data.loc[i,"Position"]
        pos_lemmatized=prepare_text(position, REtokenizer, stop_words, morpher)
        rem_token(pos_lemmatized,token1)
        rem_token(pos_lemmatized,token2)
        hsim=model.n_similarity(hdr_lemmatized, pos_lemmatized)
        header_sim.append((hsim,i))
    header_sim.sort(reverse=True)
    top_docs=[]
    if n>len(header_sim):
        n=len(header_sim)
    for i in range(0,n):
        j=header_sim[i][1]
        top_docs.append(j)
    return top_docs


# In[32]:


def get_top_texts(init_text, data, top_docs, REtokenizer, stop_words, morpher, model, n=5):
    tokens_lemmatized=prepare_text(init_text, REtokenizer, stop_words, morpher)
    sim_list=[]
    result=pd.DataFrame(index=range(0,n), columns=["Position", "Text", "File name"])
    result["Text"]=result.Text.astype('str')
    for i in top_docs:
        doc_text=data.loc[i,"Text"]
        cnt=0
        for text in doc_text:
            text_lemmatized=prepare_text(text, REtokenizer, stop_words, morpher)
            if (len(tokens_lemmatized)==0 or len(text_lemmatized)==0):
                continue
            sim=model.n_similarity(tokens_lemmatized, text_lemmatized)
            sim_list.append((sim,cnt,i))
            cnt+=1
    sim_list.sort(reverse=True)
    if n>len(sim_list):
        n=len(sim_list)
    for i in range(0,n):
        ind=sim_list[i][2]
        text_ind=sim_list[i][1]
        result.iloc[i,0]=data.loc[ind,"Position"]
        result.iloc[i,1]+=(data.loc[ind,"Text"][text_ind]+'\n')
        result.iloc[i,2]=data.loc[ind,"File name"]
    return result


# In[ ]:





# In[33]:


top_docs=get_top_docs(hdr, all_data, tokenizer, stop_words, morph, md)


# In[34]:


result=get_top_texts(init_text, all_data, top_docs, tokenizer, stop_words, morph, md)


# In[35]:


dump_data=pd.DataFrame(index=result.index, columns=["id", "Position", "Text", "path"])
dump_data["id"]=result.index.copy(deep=True)
dump_data["Position"]=result.Position.copy(deep=True)
dump_data["path"]=result["File name"].copy(deep=True)
dump_data["Text"]=result["Text"].copy(deep=True)
dump_data.to_json(path_or_buf="./docs/"+"result_table.json", orient='records')


# In[36]:


result.loc[4,"Text"]


# In[ ]:




