1`:] � �!�͈�  !H
͈\ !�
���2�
!� ^KW�w�#~�ʗ� �4#���VD����!���!  "b!D�"dͨ��t"d!D�"b� � �
� !�
4N<ʞʤ��
� �É!��=������Ɓ_ !] ��!h �w�j�t\ � <�  �2| 2�2�!  "�!f ~�Q�u�%�v�u�%���u!�6��� !t͹t�%��H�.�-��%��H�.�!|�%��H�4�%��A�� y�x��^!�
͈�t))MDD��%y��e!�6�!����!���] !���e !���!�͈:��ʴ!h͈!��z!6���������w#���!�͈�t�
¿��6#6
#6 � ��� ��!p
�!�
͈�t*b|��K�!6�p�K� !�͹��+~�	�.� �@�.�!���!6�*d���p��e~�ʴ#�N!6͈!��z!��zô~�0<w�:�60+�z^#�� �~��!^
�!D���Ҩ##^#Vz��ʺ��{/7?�7�!D�à�!�5��6�%2�:�2����%��%�lg�:���%:��� =2�:��͝�	>����2��͝��>��==2�:��*�|��=*�+"�*�~#"���!�"�!�"�6���� \ � ����� �K!"f*f "fe ͋��r>�����#��ġ�#����Q�^#V�~��#�B��Pʤèp#¹�w#���~�_��>	#�}���]���{���[������s#r��������~#�[�� �]��<� �{� � �}�,�<� �� y��<x����!^
��|��]��}ȷ��|��[��{���\��~#���M�\~�B�\�<�B�G~�ʪx�{�2�[��N#�?�p�@ʭ��pGy�aڪ�{Ҫ�_��p���y�0ڪ�:�p�Aڪ�[�p�aڪ�{�pêMD*�
�*�
�*�
�i`"�
�^#V#"�
�"�
*�
�p�$*�
ͨ�]��|��"�
���*�
��MD�"�
�"�
�"�
i`��*�
��p���MD*�
�*�
�*�
�*�
�*�
�i`"�
"�
�^#V#"�
"�
�"�
*�
�*�
�pʠ*�
�*�
�p"�
*�
"�
�_*�
ͨ�}ʛ�|"�
�l*�
� MD�"�
�"�
�"�
�"�
�"�
i`�The command line
     FYNDE D:FILE.EXT EXPRESSION
will search through all instances of FILE.EXT
(which may be an ambiguous reference) on disk D
to find lines containing EXPRESSION. Such lines
will be presented on the console preceded by a
line number, and classified by file. EXPRESSION
may have the form LABEL!PATTERN or simply the
form PATTERN. Both may contain:
     [p1!p2!...!pn]  alternative strings
     {p1!p2!...!pn}  repeated alternatives
     ? any single character
     @ for any alphanumeric: a-z, A-Z, 0-9
     _ in place of horizontal tab
When a label is present, lines will be numbered
relative to the label.  Example: X{?}:![call!ret]
will list calls and returns relative to labels
like X0100: or X33:. LABEL begins in column 1,
PATTERN can begin in any column. Squeezed files
will be searched as well as unsqueezed ones. Use
^C to quit, any other key skips rest of file.
 
FIND+ Version 2.0
 -- Bad Pattern -- 
-- Search Terminated --  -- Remainder of File Skipped --
  -- Code Table Won't Fit --
 nDFilenameEXT eezed files
will be searched as well as unsqueezed ones. Use
^C to quit, any other key skips rest of file.
 
FIND+ Version 2.0
 -- Bad Pat[original]  original.xxx
 
----> File  xxxxxxxx.xxx
       +             0
      matching lines found in this file.
       matching lines in all the files searched.
 COMOBJLBRCMDRELC0MEXEOVROVLCRLIRL                                                                           